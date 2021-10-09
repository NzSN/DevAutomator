import ast
import inspect
import astpretty
import typing as typ
import DevAuto.transFlags as flags
import DevAuto.Core as core
import DevAuto.lang_imp as dal
import DevAuto.Translator.ast_wrappers as ast_wrapper


class OUT_OF_IDENT(Exception):

    def __init__(self, ident: str) -> None:
        self._ident = ident

    def __str__(self) -> str:
        return "IdentGenerator '" + self._ident + \
            "' is out of identifiers"


class IdentGenerator:

    def __init__(self, ident: str, prefix: str, max: int) -> None:
        self._ident = ident
        self._prefix = prefix
        self._current_idx = 0
        self._max = max

    def ident(self) -> str:
        return self._ident

    def gen(self) -> str:
        if self._current_idx > self._max:
            raise OUT_OF_IDENT(self._ident)

        ident = self._prefix + "_" + str(self._current_idx)
        self._current_idx += 1

        return ident

    def set_max(self, max: int) -> None:
        self._max = max


class Translator:

    def preprocessing_before_transform(self, tree: ast.Module, info: typ.List) -> ast.AST:
        funcdef = tree.body[0]
        assert(isinstance(funcdef, ast.FunctionDef))

        # Remove decorator
        funcdef.decorator_list = []

        return tree

    def preprocessing_after_transform(self, tree: ast.Module, info: typ.List) -> ast.AST:

        funName = info[0]

        # Add Insts :: List[Inst] as an arg
        # Insts is used to hold all Insts that equal
        # to the dal.DFunc.
        funcdef = tree.body[0]
        assert(isinstance(funcdef, ast.FunctionDef))
        funcdef.args.args.append(ast.arg(arg="insts", annotation=None, type_comment=None))

        # Append a call expr
        # without this expr the pyfunc will not be executed
        # in exec()
        expr = ast.Expr(
            ast.Call(
                func = ast.Name(id=funName, ctx=ast.Load()),
                args = [ ast.Name(id='insts', ctx = ast.Load()) ],
                keywords = []))
        tree.body.append(expr)
        ast.fix_missing_locations(tree)

        return tree

    def trans(self, func: dal.DFunc) -> dal.InstGrp:
        """
        Transform a dal.DFunc into list of Insts.
        """
        pyfunc = func.body()

        target_insts = dal.InstGrp([], [], [])
        global_env = func.env()
        global_env["_da_expr_convert"] = _da_expr_convert
        loc_env = { "insts": target_insts }

        # Transform AST nodes
        ast_nodes = ast.parse(inspect.getsource(pyfunc))

        self.preprocessing_before_transform(ast_nodes, [pyfunc.__name__])

        DA_NodeTransformer(global_env).visit(ast_nodes)

        self.preprocessing_after_transform(ast_nodes, [pyfunc.__name__])

        # Transform from ast to List[Inst]
        exec(compile(ast_nodes, "", 'exec'), global_env, loc_env)

        return target_insts


class DA_NodeTransformer(ast.NodeTransformer):
    """
    Transform a dal.DFunc into intermidiate form which able
    to be execute by python interpreter to generate a list
    of DA instructions.
    """

    def __init__(self, env: typ.Dict) -> None:
        self._env = env
        self._precheck_transformer = DA_NodeTransPreCheck(env)
        self._trans_transformer = DA_NodeTransTransform(env)

    def visit(self, node: ast.AST) -> None:
        """
        Make transformations to dal.DFunc's ast nodes
        """

        # Make sure the ast is able to be transformed
        self._precheck_transformer.visit(node)

        # Do Transformation
        self._trans_transformer.visit(node)


class DA_NodeTransPreCheck(ast.NodeTransformer):
    """
    Do prechecking to ast nodes of dal.DFunc
    """

    def __init__(self, env: typ.Dict) -> None:
        ast.NodeTransformer.__init__(self)
        self._env = env


class DA_NodeTransTransform(ast.NodeTransformer):
    """
    Do transfromations to ast nodes of dal.DFunc
    """

    def __init__(self, env: typ.Dict) -> None:
        ast.NodeTransformer.__init__(self)
        self._env = env
        self._func_ident_gen = IdentGenerator("funcGen", "_lambda", 10000)

    def visit_For(self, node):
        return node

    def visit_While(self, node):
        return node

    def visit_If(self, node: ast.If):

        # Get dynamic identifier for body function and
        # elseBody function to prevent global namespace
        # conflicts
        body_func_id = self._func_ident_gen.gen()
        else_body_func_id = self._func_ident_gen.gen()

        # Wrap body of if stmt into functions
        bodyDef = ast_wrapper.function_define(body_func_id, [], node.body)
        elseBodyDef = ast_wrapper.function_define(
            else_body_func_id, [], node.orelse)

        # Transform stmts in body and elsebody recursively.
        self.visit(bodyDef)
        self.visit(elseBodyDef)

        ifCalling = ast.Expr(value=ast.Call(
            func = ast.Call(
                func = ast.Name("DIf", ctx = ast.Load()),
                args = [
                    ast.Name(id = "insts", ctx = ast.Load()),
                    node.test,
                    ast.Name(id = body_func_id, ctx = ast.Load()),
                    ast.Name(id = else_body_func_id, ctx = ast.Load())
                ],
                keywords = []
            ),
            args = [
                ast.Name(id = "_da_if_convert", ctx = ast.Load())
            ],
            keywords = []
        ))
        ast.fix_missing_locations(ifCalling)

        self._env['DIf'] = dal.DIf
        self._env['_da_if_convert'] = _da_if_convert

        return [bodyDef, elseBodyDef, ifCalling]

    def visit_Return(self, node):
        return node

    def visit_BinOp(self, node):
        return node

    def visit_Assign(self, node: ast.Assign) -> ast.Assign:
        if len(node.targets) > 1:
            raise Exception("Multiple assign is not supported")

        target = typ.cast(ast.Name, node.targets[0])
        node.value = ast.Call(
            func = ast.Name(id = "_da_assign_convert", ctx = ast.Load()),
            args = [
                ast.Name(id = "insts", ctx = ast.Load()),
                node.value,
                ast.Constant(value = target.id, kind = None)
            ],
            keywords = []
        )

        # Environment updates
        self._env["_da_assign_convert"] = _da_assign_convert

        return node

    def visit_Call(self, node: ast.Call) -> ast.Call:
        for arg in node.args:
            # Transform every argument of this call
            # into intermidiate form
            self.visit(arg)
        # To check that is any Operations is called
        # as arguments of this calling.
        node = ast_wrapper.wrap_expr_in_func(
            "_da_expr_convert", ["insts", "*"], node)
        return node


###############################################################################
#                          _da_xxx_convert functions                          #
###############################################################################
def _da_assign_convert(insts: dal.InstGrp, o: object, target: str) -> typ.Any:

    if not isinstance(o, core.Machine) and \
       not isinstance(o, core.DType):

        return o

    instCount = len(insts)

    # Convert right expr into Inst
    retValue = _da_expr_convert(insts, o)

    # A new inst is generate
    # there should only one inst is generated
    if instCount + 1 == len(insts):

        # Add target to the last generated operation
        inst = insts[-1]

        # Only support Operation Instructions
        assert(isinstance(inst, dal.OInst))

        ret = inst.ret()
        ret.ident = target
    elif instCount == len(insts):
        # It must a Dut or Executor initializes
        return o

    return retValue

def _da_expr_convert(insts: dal.InstGrp, o: object) -> typ.Any:
    """
    Convert DaObj into insts. If o is a PyObj then do nothing
    and the PyObj directly.
    """
    if isinstance(o, core.Machine):
        return _da_machine_convert(insts, o)
    elif isinstance(o, core.DType):
        return _da_oper_convert(insts, o)

    return o


def _da_machine_convert(insts: dal.InstGrp, m: core.Machine) -> core.Machine:
    """
    Generate requirements
    """
    if isinstance(m, core.Executors):
        insts.addExecutor(m.ident())
    elif isinstance(m, core.Dut):
        insts.addDut(m.ident())
    else:
        """
        A machine without extra identity
        """
        ...

    return m


def _da_oper_convert(insts: dal.InstGrp, val: core.DType) -> core.DType:
    op = val.compileInfo
    assert isinstance(op, core.Operation)

    opInfos = op.op()
    op_inst = dal.OInst(
        opInfos.opcode,
        core.DList(opInfos.opargs),
        dal.Var(""))
    insts.addInst(op_inst)
    return val


def _da_if_convert(insts: dal.InstGrp, ifStmt: dal.DIf) -> None:
    cond = ifStmt.cond()

    if isinstance(cond, bool):
        return


def _da_equal_convert(
        insts: dal.InstGrp,
        loperand: ast.expr,
        roperand: ast.expr) -> typ.Union[bool, core.DBool]:

    # Setup transform flags
    insts.setFlagT(flags.IN_IF_COND_TRANS)
    insts.setFlagF(flags.IF_COND_COMPUTEABLE_IN_PYTHON)
    insts.unsetFlag(flags.IF_COND_BOOLEAN_VALUE)

    test_ret = loperand == roperand

    if isinstance(test_ret, bool):
        insts.setFlagT(flags.IF_COND_COMPUTEABLE_IN_PYTHON)
        return test_ret
    elif isinstance(test_ret, core.DBool):

        if test_ret.isValidForm():
            # It's value is computable in python layer
            insts.setFlagT(flags.IF_COND_COMPUTEABLE_IN_PYTHON)
            return test_ret

        # DBool is not computable so need to generate
        # insts to get the boolean value of the test
        # condition.

    return True

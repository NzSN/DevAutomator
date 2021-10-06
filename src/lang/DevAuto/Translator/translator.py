import ast
import inspect
import astpretty
import typing as typ
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

        DA_NodeTransformer().visit(ast_nodes)

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

    def __init__(self) -> None:
        self._precheck_transformer = DA_NodeTransPreCheck()
        self._trans_transformer = DA_NodeTransTransform()

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


class DA_NodeTransTransform(ast.NodeTransformer):
    """
    Do transfromations to ast nodes of dal.DFunc
    """

    def __init__(self) -> None:
        ast.NodeTransformer.__init__(self)
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
        #self.visit(bodyDef
        #self.visit(elseBodyDef)

        ifCalling = ast.Call(
            func = ast.Call(
                func = ast.Name("DIf", ctx = ast.Load()),
                args = [
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
        )
        ast.fix_missing_locations(ifCalling)

        return [bodyDef, elseBodyDef, ifCalling]


    def visit_Return(self, node):
        return node

    def visit_BinOp(self, node):
        return node

    def visit_Assign(self, node: ast.Assign) -> ast.Assign:
        node.value = ast_wrapper.wrap_expr_in_func(
            "_da_expr_convert", ["insts", "*"], node.value)
        return node

    def visit_Call(self, node) -> ast.expr:
        node = ast_wrapper.wrap_expr_in_func(
            "_da_expr_convert", ["insts", "*"], node)
        return node


###############################################################################
#                          _da_xxx_convert functions                          #
###############################################################################
def _da_expr_convert(insts: dal.InstGrp, o: object) -> typ.Any:
    """
    Convert DaObj into insts. If o is a PyObj then do nothing
    and the PyObj directly.
    """
    if isinstance(o, core.Machine):
        return _da_machine_convert(insts, o)
    if isinstance(o, core.DType):
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
        core.DStr())
    insts.addInst(op_inst)
    return val


def _da_if_convert(insts: dal.InstGrp, ifStmt: dal.DIf) -> None:
    pass

import ast
import copy
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

    def environmentInit(self, func: dal.DFunc) -> typ.Dict:
        env = func.env()
        env["insts"] = dal.InstGrp([], [], [])
        env["_da_expr_convert"] = _da_expr_convert
        env["_da_unwrap"] = _da_unwrap
        env["_da_as_arg"] = _da_as_arg

        return env

    def trans(self, func: dal.DFunc) -> dal.InstGrp:
        """
        Transform a dal.DFunc into list of Insts.
        """
        pyfunc = func.body()
        global_env = self.environmentInit(func)

        # Transform AST nodes
        ast_nodes = ast.parse(inspect.getsource(pyfunc))

        self.preprocessing_before_transform(ast_nodes, [pyfunc.__name__])

        DA_NodeTransformer(global_env).visit(ast_nodes)

        self.preprocessing_after_transform(ast_nodes, [pyfunc.__name__])

        astpretty.pprint(ast_nodes)

        # Transform from ast to List[Inst]
        exec(compile(ast_nodes, "", 'exec'), global_env, {})

        return global_env["insts"]


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


class TransFlags:

    ARGUMENT_AWAIT = "arg"
    IF_TEST_EXPR = "ITE"

    def __init__(self) -> None:
        self._flagsDefault = {
            # Indicate that current expr is
            # argument of another expr
            self.ARGUMENT_AWAIT: False,

            self.IF_TEST_EXPR: None
        }  # type: typ.Dict[str, typ.Any]

        self._stack = [copy.deepcopy(self._flagsDefault)]

    def __enter__(self) -> None:
        self._recursive_in()

    def __exit__(self, exc_type, exc_value, traceback) -> None:
        self._recursive_exit()

    def current(self) -> typ.Dict[str, typ.Any]:
        return self._stack[-1]

    def get(self, key: str) -> typ.Any:
        current = self.current()

        if key in current:
            return current[key]
        else:
            # TODO: Provide a more concrete exception
            raise Exception("Key not found")

    def set(self, key: str, value: typ.Any) -> None:
        self.current()[key] = value

    def unset(self, key: str) -> None:
        self.current()[key] = None

    def setTrue(self, key: str) -> None:
        self.current()[key] = True

    def setFalse(self, key: str) -> None:
        self.current()[key] = False

    def arg_await(self) -> None:
        """
        Set arg_await flag to let Transformer to
        provide subexpr's value to parent.
        """
        self.setTrue(self.ARGUMENT_AWAIT)

    def is_arg_await(self) -> bool:
        return self.current()[self.ARGUMENT_AWAIT] == True

    def is_if_test(self) -> bool:
        return self.current()[self.IF_TEST_EXPR] == True

    def recursive(self) -> 'TransFlags':
        return self

    def _recursive_in(self) -> None:
        self._stack.append(copy.deepcopy(self._flagsDefault))

    def _recursive_exit(self) -> None:
        if len(self._stack) > 1:
            self._stack.pop()
        else:
            return

    def is_recursive_inner(self) -> bool:
        return len(self._stack) == 0


class DA_NodeTransTransform(ast.NodeTransformer):
    """
    Do transfromations to ast nodes of dal.DFunc
    """

    def __init__(self, env: typ.Dict) -> None:
        ast.NodeTransformer.__init__(self)
        self._env = env
        self._func_ident_gen = IdentGenerator("funcGen", "_lambda", 10000)
        self._flags = TransFlags()

    def decorate(self, node: ast.AST) -> typ.Optional[ast.AST]:

        new = None

        if self._flags.is_arg_await():
            # New insts is as argument of another inst.
            new = ast_wrapper.call(
                ast.Name("_da_as_arg", ctx=ast.Load()),
                [typ.cast(ast.expr, node)])

        elif self._flags.is_if_test():
            # New insts is as condition expression of
            # an Jmpxx inst.
            ...
        else:
            return node

        return new

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
        bodyDef = ast_wrapper.function_define_posonly(
            body_func_id, [], node.body)
        elseBodyDef = ast_wrapper.function_define_posonly(
            else_body_func_id, [], node.orelse)

        # Transform stmts in body and elsebody recursively.
        self.visit(bodyDef)
        self.visit(elseBodyDef)

        ifCalling = ast_wrapper.call(
            ast_wrapper.call(
                ast.Name("DIf", ctx=ast.Load()),
                [ast.Name(id="insts", ctx=ast.Load()),
                 node.test,
                 ast.Name(id=body_func_id, ctx=ast.Load()),
                 ast.Name(id=else_body_func_id, ctx=ast.Load())]
            ),
            [ast.Name(id="_da_if_convert", ctx=ast.Load())]
        )
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

        args = []

        if len(node.args) > 0:

            # Transform every argument of this call
            # into intermidiate form
            with self._flags.recursive():
                self._flags.arg_await()
                args = [self.visit(arg) for arg in node.args]

            # Replace original arguments
            # with transformed arguments.
            node.args = args

        # To check that is any Operations is called
        # as arguments of this calling.
        transform_node = typ.cast(ast.Call, ast_wrapper.parse_expr(
            "_da_call_transform(insts)"))
        transform_node.args.append(node)

        decorated_node = self.decorate(transform_node)
        if decorated_node is None:
            # TODO: Should provide a more precise exception.
            raise Exception("Failed to decorate a call expression")

        self._env["_da_call_transform"] = _da_call_transform
        return typ.cast(ast.Call, decorated_node)


###############################################################################
#                          _da_xxx_transform functions                          #
###############################################################################
class Snippet:

    def __init__(self, insts: typ.List[dal.Inst] = None, value: typ.Any = None) -> None:

        if insts is None:
            self._insts = []
        else:
            self._insts = insts
        self.value = value

    def insts(self) -> typ.List[dal.Inst]:
        return self._insts

    def addInst(self, inst: dal.Inst) -> None:
        self._insts.append(inst)

    def addInsts(self, insts: typ.List[dal.Inst]) -> None:
        for inst in insts:
            self._insts.append(inst)


def _da_as_arg(insts: dal.InstGrp, snippet: Snippet) -> Snippet:
    args = insts.getFlag(flags.CALL_ARGUMENTS)
    if args is None:
        insts.setFlagWith(flags.CALL_ARGUMENTS, [])
        args = insts.getFlag(flags.CALL_ARGUMENTS)

    assert isinstance(args, typ.List)

    for inst in snippet.insts():
        args.append(inst)

    return snippet

def _da_unwrap(snippet: Snippet) -> typ.Any:
    return snippet.value


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
        return _da_machine_transform(insts, o)
    elif isinstance(o, core.DType):
        return _da_oper_convert(insts, o)

    return o


def _da_machine_transform(insts: dal.InstGrp, m: core.Machine) -> core.Machine:
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



###############################################################################
#                          Call Expression Transform                          #
###############################################################################
class _DA_CALL_TRANSFORM_NO_ARGS_FOUND(Exception):

    def __str__(self) -> str:
        return "No argument found"


class _DA_CALL_TRANSFORM_ARGS_MISMATCH(Exception):

    def __str__(self) -> str:
        return "argument mismatch"



def _da_call_transform(insts: dal.InstGrp, o: typ.Any) -> Snippet:

    snippet = Snippet(value=o)

    if not isinstance(o, core.DType):
        # Value that is computable in python layer
        return o
    elif isinstance(o, core.Machine):
        # Need to generate Dut, Executor description.
        # For example:
        #   box = BoxMachine()
        # It's a call but not an operation.
        _da_machine_transform(insts, o)

    op = o.compileInfo
    if op is None:
        # Which means this Value is not generated by
        # operation so no need to convert.
        return snippet

    assert isinstance(op, core.Operation)

    opInfo = op.op()
    args = []
    argv = len(opInfo.opargs)

    # Get Arguments if need
    if argv > 0:
        args = insts.getFlag(flags.CALL_ARGUMENTS)

        if args is None:
            raise _DA_CALL_TRANSFORM_NO_ARGS_FOUND()
        if len(args) != argv:
            raise _DA_CALL_TRANSFORM_ARGS_MISMATCH()

        insts.unsetFlag(flags.CALL_ARGUMENTS)

    op_inst = dal.OInst(
        opInfo.opcode,
        core.DList(args),
        dal.Var()
    )
    insts.addInst(op_inst)
    snippet.addInst(op_inst)

    return snippet


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

import ast
import inspect
import typing as typ
import DevAuto.Core as core
import DevAuto.lang_imp as dal
import DevAuto.Translator.ast_wrappers as ast_wrapper


class Translator:

    def preprocessing(self, tree: ast.Module, info: typ.List) -> ast.AST:

        funName = info[0]

        # Add Insts :: List[Inst] as an arg
        # Insts is used to hold all Insts that equal
        # to the dal.DFunc.
        funcdef = tree.body[0]
        assert(isinstance(funcdef, ast.FunctionDef))
        funcdef.args.args.append(ast.arg(arg="insts"))

        # Remove decorator
        funcdef.decorator_list = []

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
        DA_NodeTransformer().visit(ast_nodes)

        self.preprocessing(ast_nodes, [pyfunc.__name__])

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

    def visit_For(self, node):
        return node

    def visit_While(self, node):
        return node

    def visit_If(self, node):
        return node

    def visit_Return(self, node):
        return node

    def visit_BinOp(self, node):
        return node

    def visit_Compare(self, node):
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
    op_inst = dal.OInst(opInfos.opcode,
                        core.DList(opInfos.opargs),
                        core.DStr())
    insts.addInst(op_inst)
    return val

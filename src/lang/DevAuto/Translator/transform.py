# TODO: Need to setup DType.TransformInfos during transforming.

import ast
import typing as typ
import DevAuto.Core as core
import DevAuto.lang_imp as dal
import DevAuto.transFlags as flags
from .trans_utilities import TransformInfos


class Snippet:

    def __init__(self, insts: typ.List[typ.Any] = None, value: typ.Any = None) -> None:

        if insts is None:
            self._insts = []
        else:
            self._insts = insts
        self.value = value

    def insts(self) -> typ.List[typ.Union[dal.Inst, str]]:
        return self._insts

    def addInst(self, inst: typ.Union[dal.Var, str]) -> None:
        self._insts.append(inst)

    def addInsts(self, insts: typ.List[dal.Inst]) -> None:
        for inst in insts:
            self._insts.append(inst)



###############################################################################
#                                  Modifiers                                  #
###############################################################################
def da_define(insts: dal.InstGrp, identifier: str, var: core.DType) -> Snippet:
    """
    Note: This modifier should be only used while Variable's value is
    unpredictable.

    For example:

    def f():
        if expr:
            v = da_unwrap(da_define("v", da_call_transform(DInt(1))))
        else:
            v = da_unwrap(da_define("v", da_call_transform(DInt(2))))

        f(v)

    v is unknowk until expr is evaluated, so need to bind DInt(1) and DInt(2)'s
    Def Inst to the same variable.
    """
    s = Snippet(value=var)

    # To check that is variable is already defined
    da_var_ident = insts.get_da_var(identifier)
    if da_var_ident is None:
        da_var_ident = insts.new_da_var_ident()

    insts.addInst(dal.Def(da_var_ident, var.value()))

    return s

def da_as_arg(insts: dal.InstGrp, snippet: Snippet) -> Snippet:

    args = insts.getFlag(insts.ARG_HOLDER)
    assert isinstance(args, typ.List)

    for inst in snippet.insts():
        args.append(inst)

    return snippet


def da_as_assign_value(insts: dal.InstGrp, snippet: Snippet) -> Snippet:

    # Both da_as_arg and da_as_assign_value use same entry
    # pass Variable so able to reuse da_as_arg
    da_as_arg(insts, snippet)

    return snippet


def da_unwrap(o: typ.Any) -> typ.Any:

    if isinstance(o, Snippet):
        return o.value
    else:
        return o


###############################################################################
#                             Transform Functions                             #
###############################################################################
def da_constant_transform(insts: dal.InstGrp, const: core.DType) -> Snippet:
    snippet = Snippet(value=const)

    # To check that whether bind to a variable.
    if not const.transInfo is None:
        var_ident = const.transInfo.var_identifier()
        if not var_ident is None:
            snippet.addInst(dal.Var(var_ident, const.value()))
            return snippet

    snippet.addInst(const.value())

    return snippet


def da_transform_check(o: object) -> bool:

    # If the operation was already transformed
    if isinstance(o, core.DType):
        infos = o.transInfo

        if infos is None:
            # A DA Constant
            return True
        elif infos.is_transformed():
            return False

    return True


def da_name_transform(insts: dal.InstGrp, n: typ.Any) -> Snippet:
    s = Snippet(value=n)

    if not isinstance(n, core.DType):
        return s
    else:

        if n.compileInfo is None:
            return da_constant_transform(insts, n)

        ret = n.transInfo.op_ret()
        if ret is None:
            raise Exception("da_name_transform: ret not found")

        s.addInst(ret)

    return s


def da_assign_transform(insts: dal.InstGrp, target_ident: str, target: object) -> None:
    """
    Transform assignment into Instructions if need.
    """

    if isinstance(target, core.Machine):
        return

    # Cause DAL is not an OOP language, there is no things like objects,
    # structure, and also cause of it still no any container currently
    # so generate Instructions only when value is an operation.
    if isinstance(target, core.DType):

        if target.compileInfo is None:
            # Assignment's value expression is
            # not an Machine Operation just return
            return

        value_expr_var = insts.getFlag(insts.ARG_HOLDER)
        insts.setFlagWith(insts.ARG_HOLDER, [])
        assert value_expr_var is not None
        if len(value_expr_var) != 1:
            raise Exception("Assignment Transform failed to get right expression")

        # otherwise it's a Machine Operation then redirect it's
        # result to a DA Variable in DAL layer.
        insts.addInst(dal.Assign(target_ident, value_expr_var[0]))

    return


def da_expr_convert(insts: dal.InstGrp, o: object) -> typ.Any:
    """
    Convert DaObj into insts. If o is a PyObj then do nothing
    and the PyObj directly.
    """
    if isinstance(o, core.Machine):
        return da_machine_transform(insts, o)
    elif isinstance(o, core.DType):
        return da_oper_convert(insts, o)

    return o


def da_machine_transform(insts: dal.InstGrp, m: core.Machine) -> core.Machine:
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
class DA_CALL_TRANSFORM_NO_ARGS_FOUND(Exception):

    def __str__(self) -> str:
        return "No argument found"


class DA_CALL_TRANSFORM_ARGS_MISMATCH(Exception):

    def __str__(self) -> str:
        return "argument mismatch"


def da_call_not_operation(insts: dal.InstGrp, o: typ.Any) \
    -> typ.Optional[Snippet]:

    snippet = Snippet(value=o)

    if isinstance(o, core.Machine):
        # Need to generate Dut, Executor description.
        # For example:
        #   box = BoxMachine()
        # It's a call but not an operation.
        da_machine_transform(insts, o)
        return snippet

    elif not isinstance(o, core.DType):
        # Value that is computable in python layer
        return snippet

    assert(isinstance(o, core.DType))

    op = o.compileInfo

    if op is None:
        # A DA Constant
        return da_constant_transform(insts, o)

    return None


def da_call_transform(insts: dal.InstGrp, o: typ.Any) -> Snippet:

    transInfos = o.transInfo = TransformInfos()

    snippet = da_call_not_operation(insts, o)
    if not snippet is None:
        return snippet

    snippet = Snippet(value=o)

    assert(isinstance(o, core.DType))

    op = o.compileInfo

    assert(isinstance(transInfos, TransformInfos))
    assert(isinstance(op, core.Operation))

    opInfo = op.op()
    args = []
    argv = len(opInfo.opargs)

    # Get Arguments if need
    if argv > 0:
        args = insts.getFlag(insts.ARG_HOLDER)

        if args is None:
            raise DA_CALL_TRANSFORM_NO_ARGS_FOUND()
        if len(args) != argv:
            raise DA_CALL_TRANSFORM_ARGS_MISMATCH()

        insts.setFlagWith(insts.ARG_HOLDER, [])

    retVar = insts.compileDict[insts.VAR_ID_GEN].gen()
    var = dal.Var(retVar)

    op_inst = dal.OInst(
        opInfo.opcode,
        core.DList(args),
        var
    )
    insts.addInst(op_inst)
    snippet.addInst(var)

    transInfos.set_op_ret(var)
    transInfos.transformed()

    return snippet


def da_oper_convert(insts: dal.InstGrp, val: core.DType) -> core.DType:
    op = val.compileInfo
    assert isinstance(op, core.Operation)

    opInfos = op.op()
    op_inst = dal.OInst(
        opInfos.opcode,
        core.DList(opInfos.opargs),
        dal.Var(""))
    insts.addInst(op_inst)
    return val


def da_if_convert(insts: dal.InstGrp, ifStmt: dal.DIf) -> None:

    cond = ifStmt.cond()

    if isinstance(cond, bool):
        return


def da_equal_convert(
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

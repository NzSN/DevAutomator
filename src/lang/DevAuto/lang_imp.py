import typing as typ
import DevAuto.Core as core

T = typ.TypeVar("T")


class DFunc:
    """
    DevAuto Function
    """

    def __init__(self, func: typ.Callable, env: typ.Dict) -> None:
        self.__name__ = func.__name__
        self._func = func
        self.__IS_DA_FUNC__ = True
        self._env = env

    def body(self) -> typ.Callable:
        return self._func

    def env(self) -> typ.Dict:
        return self._env

    def __call__(self) -> 'DFunc':
        return self


class DIf:
    """
    IF statement of DevAuto.
    """


class DWhile:
    """
    While statement of DevAuto
    """


class DFor:
    """
    For statement of DevAuto
    """


class Var:

    def __init__(self, value: typ.Any) -> None:
        self.value = value


class Inst:

    def __init__(self, icode: str) -> None:
        self._inst_code = icode

    def code(self) -> str:
        return self._inst_code


class CInst(Inst):

    COND_INST = "CInst"

    def __init__(self, var: Var, dest: core.DInt) -> None:
        Inst.__init__(self, self.COND_INST)
        self._test = var
        self._dest = dest

    def cond(self) -> Var:
        return self._test

    def dest(self) -> core.DInt:
        return self._dest


class OInst(Inst):

    OPER_INST = "OInst"

    def __init__(self, opcode: str,
                 args: core.DList[core.DStr],
                 ret: core.DStr) -> None:

        Inst.__init__(self, self.OPER_INST)
        self._opcode = opcode
        self._args = args
        self._ret = ret

    def opcode(self) -> str:
        return self._opcode

    def args(self) -> core.DList:
        return self._args

    def ret(self) -> core.DStr:
        return self._ret


class VInst(Inst):

    VAR_OP_INST = "VInst"

    def __init__(self, l: typ.Union[str, Var], r:typ.Union[str, Var]) -> None:
        Inst.__init__(self, self.VAR_OP_INST)
        self._l = l
        self._r = r

    def loperand(self) -> typ.Union[str, Var] :
        return self._l

    def roperand(self) -> typ.Union[str, Var]:
        return self._r


class Term(Inst):

    TERM_INST = "TERM"

    def __init__(self) -> None:
        Inst.__init__(self, self.TERM_INST)


class Success(Term):

    def __init__(self) -> None:
        Term.__init__(self)


class Fail(Term):

    def __init__(self) -> None:
        Term.__init__(self)


class Jmp(CInst):

    def __init__(self, cond: Var, to: core.DInt) -> None:
        CInst.__init__(self, cond, to)
        self._cond = cond
        self._to = to

    def cond(self) -> Var:
        return self._cond

    def to(self) -> core.DInt:
        return self._to


class Op(OInst):

    def __init__(self, opcode: str, args: core.DList[core.DStr], ret: core.DStr) -> None:
        OInst.__init__(self, opcode, args, ret)


class Equal(VInst):

    def __init__(self, l: typ.Union[str, Var], r: typ.Union[str, Var]) -> None:
        VInst.__init__(self, l, r)


class InstGrp:
    """
    InstGrp is a list of Insts with extra informations that
    explain which executor and duts is need.
    """

    def __init__(self, insts: typ.List[Inst],
                 duts: typ.List[str],
                 executors: typ.List[str]) -> None:

        self._insts = insts
        self._duts = duts
        self._executors = executors

    def insts(self) -> typ.List[Inst]:
        return self._insts

    def duts(self) -> typ.List[str]:
        return self._duts

    def executors(self) -> typ.List[str]:
        return self._executors


def function(env):
    """
    Decorator that mark a python function as a DA function.
    """
    def deco(f: typ.Callable) -> DFunc:
        return DFunc(f, env)

    return deco

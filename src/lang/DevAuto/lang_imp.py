import functools
import typing as typ
import DevAuto.Core as core


class DFunc:
    """
    DevAuto Function
    """

    def __init__(self, func: typ.Callable) -> None:
        self.__name__ = func.__name__
        self._func = func
        self.__IS_DA_FUNC__ = True

    def body(self) -> typ.Callable:
        return self._func

class DIf:
    """
    IF statement of DevAuto.
    """


class DWhile:
    """
    While statement of DevAuto
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
                 args: typ.List[core.DStr],
                 ret: core.DStr) -> None:

        Inst.__init__(self, self.OPER_INST)
        self._opcode = opcode
        self._args = args
        self._ret = ret

    def opcode(self) -> str:
        return self._opcode

    def args(self) -> typ.List[core.DStr]:
        return self._args

    def ret(self) -> core.DStr:
        return self._ret


class Jmp(CInst):

    def __init__(self, cond: Var, to: core.DInt) -> None:
        CInst.__init__(self, cond, to)
        self._cond = cond
        self._to = to

    def cond(self) -> Var:
        return self._cond

    def to(self) -> core.DInt:
        return self._to


def function(t: typ.Callable) -> DFunc:
    """
    Decorator that mark a python function as a DA function.
    """
    return DFunc(t)

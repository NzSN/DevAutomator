import typing as typ


class Var:

    def __init__(self, value: typ.Any) -> None:
        self.value = value


class Inst:

    def __init__(self, code: str) -> None:
        self._inst_code = code


class Jmp(Inst):

    def __init__(self, cond: Var, to: int) -> None:
        self._cond = cond
        self._to = to

    def cond(self) -> Var:
        return self._cond

    def to(self) -> int:
        return self._to


class Translator:

    pass

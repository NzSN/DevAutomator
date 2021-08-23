import typing as typ
from collections import namedtuple

opTuple = namedtuple("opTuple", "opcode opargs opret")
opParameter = typ.Tuple[typ.Tuple[str, type]]
opRet = opParameter
PropVal = typ.Union[typ.List[str], typ.Mapping[str, str]]


###############################################################################
#                                   DA Types                                  #
###############################################################################
class DType:

    def __init__(self, t: type) -> None:
        self._origin = t
        self._value = None  # type: any


class DINT(DType):

    def __init__(self, value: int) -> None:
        DType.__init__(self, int)
        self._value = value


class DSTR(DType):

    def __init__(self, value: str) -> None:
        DType.__init__(self, str)
        self._value = value


class DLIST(DType):

    def __init__(self, value: typ.List) -> None:
        DType.__init__(self, list)
        self._value = value


class DDICT(DType):

    def __init__(self, value: typ.Dict) -> None:
        DType.__init__(self, dict)
        self._value = value


class DTuple(DType):

    def __init__(self, value: typ.Tuple) -> None:
        DType.__init__(self, tuple)
        self._value = value


class DNone(DType):

    def __init__(self) -> None:
        DType.__init__(self, None)
        self._value = None

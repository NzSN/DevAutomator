import typing as typ
from collections import namedtuple


PropVal = typ.Union[typ.List[str], typ.Mapping[str, str]]


###############################################################################
#                                   DA Types                                  #
###############################################################################
class DType:

    def __init__(self, t: typ.Union[type, None]) -> None:
        self._origin = t
        self._value = None  # type: typ.Any


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


###############################################################################
#                                  Operation                                  #
###############################################################################
# opargs :: opParameter
opTuple = namedtuple("opTuple", "opcode opargs")
opParameter = typ.Tuple[typ.Tuple[str, DType]]
opRet = typ.Tuple[str, DType]


def paraMatch(para1: opParameter, para2: opParameter) -> bool:
    length = len(para1)
    if length != len(para2):
        return False

    # If two parameter's type is equal then
    # these two parameter is equal
    for i in range(length):
        if para1[i][1] != para2[i][1]:
            return False

    return True

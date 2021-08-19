import typing as typ
from collections import namedtuple

opTuple = namedtuple("opTuple", "opcode opargs opret")
opParameter = typ.Tuple[typ.Tuple[str, type]]
opRet = opParameter
PropVal = typ.Union[typ.List[str], typ.Mapping[str, str]]


###############################################################################
#                                   DA Types                                  #
###############################################################################
class DA_Type:
    pass


class DA_INT(DA_Type):
    pass


class DA_STR(DA_Type):
    pass


class DA_LIST(DA_Type):
    pass


class DA_DICT(DA_Type):
    pass

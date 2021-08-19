import typing as typ
from collections import namedtuple

opTuple = namedtuple("opTuple", "opcode opargs")
PropVal = typ.Union[typ.List[str], typ.Mapping[str, str]]

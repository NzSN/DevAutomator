# DevCore is collection of fundamental objects in DevAuto.

import typing as typ
from collections import namedtuple


class Message:
    """
    Communication unit between DaCore and Executors, Duts. All
    things between DACore and Executors, Duts are Messages.
    """

    def __init__(self, source: str, dest: str, content: str) -> None:
        self._source = source
        self._dest = dest
        self._content = content

    def source(self) -> str:
        return self._source

    def dest(self) -> str:
        return self._dest

    def content(self) -> str:
        return self._content


opTuple = namedtuple("opTuple", "opcode opargs")


class Operation(Message):
    """
    Operation is a kind of message that order executors to do a specific
    action.

    Operation string format:
    opcode :: opargs :: ... (... means not not specified)
    """

    def __init__(self, source: str, dest: str,
                 op: opTuple) -> None:

        self._op = op

        # Build up operation content
        content = op[0] + "::" + op[1]

        Message.__init__(self, source, dest, content)

    def op(self) -> opTuple:
        return self._op

    def __eq__(self, op: 'Operation') -> bool:
        return self._op == op.op()


class Config(Operation):
    """
    Config is a kind of Operation that use to config Executors, Duts, no
    infomations is return that cause by config except an acknowledge which
    indicate wheter target is config properly.

    With config indicater to indicate that the operation will change
    some properties of target.

    Operation string format:
    opcode :: opargs :: <config indicater>
    """

    config_indicater = "cfg"

    def __init__(self, source: str, dest: str,
                 op: typ.Tuple[str]) -> None:
        Operation.__init__(self, source, dest, op)

        # Append config indicater to tail of operation
        self._content = self._content + "::" + self.config_indicater


class Query(Operation):
    """
    Query is a kind of Operation that retrive informations from target,
    without properties, status changed to target.

    Like Config it need a query indicater.

    Operation string format:
    opcode :: opargs :: <query indicater>
    """

    query_indicater = "query"

    def __init__(self, source: str, dest: str,
                 op: opTuple) -> None:
        Operation.__init__(self, source, dest, op)

        # Append query indicater to tail of operation
        self._content = self._content + "::" + self.query_indicater


class Property:
    """
    Property is part of machine, it show that what feature a machine has,
    on python layer, it just an unique string within a machine.
    """

    def __init__(self, p: str, *propval: typ.List[str]) -> None:
        self._property = p
        self._propVal = propval

    def __eq__(self, property: 'Property') -> bool:
        return (self._property, self._propVal) == \
            (property._property, property._propVal)

    def __str__(self) -> str:
        return self._property


class OpSpec:
    """
    An specification of operations.
    """

    def __init__(self, opcode: str,
                 para: typ.Tuple[str], ret: typ.Tuple[str]) -> None:
        self._opcode = opcode
        self._para = para
        self._ret = ret

    def __eq__(self, spec: 'OpSpec') -> bool:
        return (self._opcode, self._para, self._ret) == \
            (spec._opcode, spec._para, spec._ret)

    def opcode(self) -> str:
        return self._opcode

    def parameter(self) -> typ.Tuple[str]:
        return self._para

    def retVal(self) -> typ.Tuple[str]:
        return self._ret


class OPERATION_NOT_DEFINED(Exception):

    def __init__(self, machine: str, opName: str) -> None:
        self._machine = machine
        self._opName = opName

    def __str__(self) -> str:
        return "Operation " + self._opName + " is not define in machine " + \
            self._machine


class Machine:
    """
    An entity that able to perform operations and have
    some properties. To specify a more concrete machine by
    extend this class.
    """

    def __init__(self, ident: str, properties: typ.List[Property],
                 operations: typ.List[OpSpec]) -> None:
        self._ident = ident
        self._properties = properties
        self._operations = operations

    def hasProperty(self, pident: str) -> bool:
        theProperty = [pident == p for p in self._properties]
        return theProperty != []

    def hasOperation(self, opcode: str) -> bool:
        theOp = [op.op()[0] == opcode for op in self._operations]
        return theOp != []

    def operation(self, op: typ.Callable) -> typ.Callable:
        if not self.hasOperation(op.__name__):
            # fixme: need to raise exception
            raise OPERATION_NOT_DEFINED(self._ident, op.__name__)
        else:
            return op

"""
This file provide an example of how to make a more concrete macine
from Machine within DevCore.
"""

import devCore as core


###############################################################################
#                                TrivialMachine                               #
###############################################################################
class TrivialMachine(core.Machine):
    """
    A machine without properties, operations, it's trivial.
    """

    def __init__(self) -> None:
        core.Machine.__init__(self, "Trivial", [], [])


###############################################################################
#                  BoxMachine, a machine that simulate a box                  #
###############################################################################
# Box Properties
boxProperties = [
    core.Property("contain", 10)
]

boxOpSpec = [
    # Open
    core.OpSpec("open", (), None),
    # Close
    core.OpSpec("close", (), None),
    # Put
    core.OpSpec("put", ("Candy"), None),
    # Get
    core.OpSpec("get", (), ("Candy"))
]


class BoxMachine(core.Machine):
    """
    A machine that can open and close, just like a box.
    """

    def __init__(self) -> None:
        core.Machine.__init__(self, "Box", boxProperties, boxOpSpec)

    @core.Machine.Operation
    def open(self) -> core.Operation:
        return core.Operation("core", "Box", core.opTuple("open", None))

    @core.Machine.Operation
    def close(self) -> core.Operation:
        return core.Operation("core", "Box", core.opTuple("close", None))

    @core.Machine.Operation
    def put(self, candy: str) -> core.Operation:
        return core.Operation("core", "Box", core.opTuple("put", candy))

    @core.Machine.Operation
    def get(self) -> core.Operation:
        return core.Operation("core", "Box", core.opTuple("get", None))


###############################################################################
#                      Network Device with four interface                     #
###############################################################################
NA = "N/A"
NDProp = [
    core.Property("Interface",
                  "interface-0/1", "interface-0/2",
                  "interface-0/3", "interface-0/4"),
    core.Property("Interface-State", {
        "interface-0/1": NA,
        "interface-0/2": NA,
        "interface-0/3": NA,
        "interface-0/4": NA
    })
]

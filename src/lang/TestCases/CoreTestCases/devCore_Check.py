import pytest
import DevAuto.Core as core


###############################################################################
#                              Message TestCases                              #
###############################################################################
@pytest.fixture
def Trivial() -> core.Message:
    return core.Message("S", "D", "MSG")


class Message_TC:

    def test_Message(self, Trivial) -> None:
        assert Trivial.source() == "S"
        assert Trivial.dest() == "D"
        assert Trivial.content() == "MSG"


###############################################################################
#                             Operation TestCases                             #
###############################################################################
@pytest.fixture
def trivialOP() -> core.Operation:
    return core.Operation("S", "D", ("OP", "ARG"))


class Operation_TC:

    def test_OP(self, trivialOP) -> None:
        assert ("OP", "ARG") == trivialOP.op()


###############################################################################
#                              Property TestCases                             #
###############################################################################
@pytest.fixture
def ListProperty() -> core.Property:
    return core.Property("LP", ["0/1", "0/2", "0/3"])


@pytest.fixture
def DictProperty() -> core.Property:
    return core.Property("DP", {
        "0/1": "on",
        "0/2": "off",
        "0/3": "on"
    })


class PropertyAsList_TC:

    def test_GetItem(self, ListProperty) -> None:
        conds = ["0/1", "0/2", "0/3"]

        for idx in range(2):
            assert conds[idx] == ListProperty[idx]

    def test_SetItem(self, ListProperty) -> None:
        alterTo = ["1", "2", "3"]

        for idx in range(2):
            ListProperty[idx] = alterTo[idx]
            assert ListProperty[idx] == alterTo[idx]

        for idx in range(2):
            assert ListProperty[idx] == alterTo[idx]


class PropertyAsDict_TC:

    def test_GetItem(self, ListProperty, DictProperty) -> None:
        conds = ["on", "off", "on"]
        for idx in range(2):
            port = ListProperty[idx]
            assert conds[idx] == DictProperty[port]

    def test_SetItem(self, ListProperty, DictProperty) -> None:
        alterTo = ["off", "on", "off"]
        for idx in range(2):
            port = ListProperty[idx]
            DictProperty[port] = alterTo[idx]
        for idx in range(2):
            port = ListProperty[idx]
            assert alterTo[idx] == DictProperty[port]


###############################################################################
#                              Machine TestCases                              #
###############################################################################
# Box Properties
boxProperties = [
    core.Property("contain", 10)
]

boxOpSpec = [
    # Open
    core.OpSpec("open", (), ("N/A", core.DNone)),
    # Close
    core.OpSpec("close", (), ("N/A", core.DNone)),
    # Put
    core.OpSpec("put", (("Candy", core.DStr)), ("N/A", core.DNone)),
    # Get
    core.OpSpec("get", (), ("Candy", core.DStr))
]


class BoxMachine(core.Machine):
    """
    A machine that can open and close, just like a box.
    """

    def __init__(self) -> None:
        core.Machine.__init__(self, "Box", boxProperties, boxOpSpec)

    @core.Machine.operation("BoxMachine", boxOpSpec)
    def open(self) -> core.DNone:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("open", [])
            )
        )

    @core.Machine.operation("BoxMachine", boxOpSpec)
    def close(self) -> core.DNone:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("close", [])
            )
        )

    @core.Machine.operation("BoxMachine", boxOpSpec)
    def put(self, candy: str) -> core.DNone:
        arg = core.DStr(candy)

        return self.operate(
            "core", "Box",
            core.opTuple("put", [arg])
        )

    @core.Machine.operation("BoxMachine", boxOpSpec)
    def get(self) -> core.DStr:
        return self.operate(
            "core", "Box",
            core.opTuple("get", [])
        )


@pytest.fixture
def boxMachine() -> core.Machine:
    return BoxMachine()


class Machine_TC:

    def test_BoxMachineOpSpec(self, boxMachine) -> None:
        assert boxMachine.hasOperation("open")
        assert boxMachine.hasOperation("close")
        assert boxMachine.hasOperation("put")
        assert boxMachine.hasOperation("get")

    def test_BoxMachineProperty(self, boxMachine) -> None:
        pass

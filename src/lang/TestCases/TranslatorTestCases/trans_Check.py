import pytest
import DevAuto as DA
import DevAuto.Core as core
import DevAuto.Translator.translator as trans
import DevAuto.lang_imp as dal
from DevAuto import DFunc
from DevAuto.lang_imp import Inst, InstGrp
from TestCases.CoreTestCases.devCore_Check import BoxMachine


boxOpSpec = [
    # is_open
    core.OpSpec("is_open", [], ("successed", core.DBool)),
]


class BoxMachinePlus(BoxMachine, core.Dut):

    def __init__(self) -> None:
        BoxMachine.__init__(self)
        self._operations.append(
            core.OpSpec("is_open", [], ("successed", core.DBool))
        )

    @core.Machine.operation("is_open", boxOpSpec)
    def is_open(self) -> core.DBool:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("is_open", [])
            )
        )


@DA.function(globals())
def trivialTest() -> bool:
    box = BoxMachinePlus()

    box.open()

    box.put("candy")
    things = box.get()

    if things == "candy":
        return True
    else:
        return False


@pytest.fixture
def Tr() -> trans.Translator:
    return trans.Translator()


@pytest.fixture
def TrivialTest() -> DFunc:
    return trivialTest


class Tr_TC:

    def test_TR(self, Tr, TrivialTest) -> None:
        instgrp = Tr.trans(TrivialTest)  # type: InstGrp

        assert instgrp is not None

        # Verify that insts is successful generated
        # box = BoxMachinePlus()
        assert instgrp.duts() == ["Box"]

        insts = instgrp.insts()

        assert insts[0] == \
            dal.OInst("open", core.DList(), dal.Var(""))

        # box.put("candy")
        assert insts[1] == \
            dal.OInst("put", core.DList(), dal.Var(""))

        # if things == "candy":
        #     return True
        # else:
        #     return False
        assert insts[2] == \
            dal.OInst("get", core.DList(), dal.Var("things"))

import pytest
import DevAuto as DA
import DevAuto.Core as core
import DevAuto.Translator.translator as trans
from DevAuto import DFunc
from DevAuto.lang_imp import Inst
from TestCases.CoreTestCases.devCore_Check import BoxMachine


class BoxMachinePlus(BoxMachine):

    def __init__(self) -> None:
        BoxMachine.__init__(self)

    def is_open(self) -> core.DBool:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("is_open", [])
            )
        )


@DA.function
def trivialTest() -> bool:
    box = BoxMachinePlus()

    box.open()
    if not box.is_open():
        return False

    box.put("candy")
    things = box.get()

    if things == "candy":
        return True


@pytest.fixture
def Tr() -> trans.Translator:
    return trans.Translator()


@pytest.fixture
def TrivialTest() -> DFunc:
    return trivialTest


class Tr_TC:

    def test_TR(self, Tr, TrivialTest) -> None:
        insts = Tr.trans(TrivialTest)

        assert insts is not None

        # Verify that insts is successful generated
        for inst in insts:
            assert isinstance(inst, Inst)

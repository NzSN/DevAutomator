import pytest
import DevAuto as DA
import DevAuto.Translator.translator as trans
from DevAuto import DFunc
from DevAuto.lang_imp import Inst


@DA.function
def trivialTest() -> None:
    pass


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

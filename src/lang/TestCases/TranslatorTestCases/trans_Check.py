import pytest
import DevAuto as DA
import DevAuto.Translator.translator as translator
from DevAuto import DFunc


@DA.function
def trivialTest() -> None:
    pass


@pytest.fixture
def Tr() -> translator:
    return translator()


@pytest.fixture
def TrivialTest() -> DFunc:
    return trivialTest



class Tr_TC:

    def test_TR(self, Tr, TrivialTest) -> None:
        insts = Tr.trans(TrivialTest)

        # Verify that insts is successful generated

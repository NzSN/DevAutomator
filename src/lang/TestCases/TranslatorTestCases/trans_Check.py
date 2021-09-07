import pytest
import DevAuto as DA
import DevAuto.Translator.translator as translator, DAL


@pytest.fixture
def Tr() -> translator:
    return translator()


@DA.function
def trivialTest() -> None:
    pass



@pytest.fixture
def TrivialTest() -> DFunc:
    return trivialTest



class Tr_TC:

    def test_TR(self, Tr, TrivialTest) -> None:
        dal = Tr.trans(TrivialTest)
        # Verify
        """
        .......
        """

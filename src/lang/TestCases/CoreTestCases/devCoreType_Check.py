import pytest
import DevAuto.Core as core
import typing as typ


###############################################################################
#                        Operation parameter TestCases                        #
###############################################################################@
@pytest.fixture
def parameter_pair_diff() -> typ.Tuple[core.opParameter, core.opParameter]:
    return (
        (("P1", ), ("P2", str), ("P3", list))
    )


@pytest.fixture
def parameter_pair_equal() -> typ.Tuple[core.opParameter, core.opParameter]:
    pass


class OperationParameter_TC:

    def test_ParameterMatchDiff(self) -> None:
        pass

    def test_ParameterMatchEqual(self) -> None:
        pass

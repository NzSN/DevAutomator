import pytest

###############################################################################
#                              Property TestCases                             #
###############################################################################
import DevAuto.Core as core


@pytest.fixture
def ListProperty():
    return core.Property("LP", ["0/1", "0/2", "0/3"])


@pytest.fixture
def DictProperty():
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

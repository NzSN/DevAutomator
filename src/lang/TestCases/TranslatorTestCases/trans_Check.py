import pytest
import typing as typ
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
    # op
    core.OpSpec("op", [("arg", core.DStr)], ("ret", core.DNone)),
    # query
    core.OpSpec("query", [("arg", core.DStr)], ("ret", core.DStr))
]


class BoxMachinePlus(BoxMachine, core.Dut):

    def __init__(self) -> None:
        BoxMachine.__init__(self)
        self._operations = self._operations + boxOpSpec

    @core.Machine.operation("is_open", boxOpSpec)
    def is_open(self) -> core.DBool:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("is_open", [])
            )
        )

    @core.Machine.operation("op", boxOpSpec)
    def op(self, arg: core.DStr) -> None:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("op", [arg])
            )
        )

    @core.Machine.operation("query", boxOpSpec)
    def query(self, arg: core.DStr) -> core.DStr:
        return self.operate(
            core.Operation(
                "core",
                "Box",
                core.opTuple("query", [arg])
            )
        )


@DA.function(globals())
def CallExpression_Case_1() -> bool:
    box = BoxMachinePlus()
    box.op(box.query(core.DStr("thing")))

    return True

@pytest.fixture
def Tr() -> trans.Translator:
    return trans.Translator()


@pytest.fixture
def CallExpression_Cases() -> typ.List[DFunc]:
    tests = [CallExpression_Case_1]
    return tests


class Tr_TC:

    def test_Call_Expression_Transform(self, Tr, CallExpression_Cases) -> None:
        instgrp = Tr.trans(CallExpression_Cases[0])  # type: InstGrp

        assert instgrp is not None

        # Verify that insts is successful generated
        # box = BoxMachinePlus()
        assert instgrp.duts() == ["Box"]

        insts = instgrp.insts()

        assert 1 == 2

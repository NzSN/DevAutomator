import pytest
import astpretty
import typing as typ
import DevAuto as DA
import DevAuto.Core as core
from DevAuto.Core import DStr
import DevAuto.Translator.translator as trans
from DevAuto import DFunc
from DevAuto.lang_imp import InstGrp
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
        self.value = 0

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


###############################################################################
#                           Call Expressoin Fixtures                          #
###############################################################################
@DA.function(globals())
def CallExpression_Case_1() -> bool:
    box = BoxMachinePlus()
    box.op(box.query(DStr("things")))

    return True


@DA.function(globals())
def CallExpression_Case_2() -> bool:
    box = BoxMachinePlus()
    box.op(DStr("thing"))
    return True


@pytest.fixture
def CallExpression_Cases() -> typ.List[DFunc]:
    tests = [CallExpression_Case_1]
    return tests


###############################################################################
#                          Assign Statement Fixtures                          #
###############################################################################
@DA.function(globals())
def AssignStmts_Case_1() -> bool:
    box = BoxMachinePlus()
    info = box.query(DStr("things"))
    return True


@DA.function(globals())
def AssignStmts_Case_2() -> bool:
    info = 1
    return True


@DA.function(globals())
def AssignStmts_Case_3() -> bool:
    box = BoxMachinePlus()
    box.value = 1
    return True


@pytest.fixture
def AssignStmts_Cases() -> typ.List[DFunc]:
    return [AssignStmts_Case_1, AssignStmts_Case_2, AssignStmts_Case_3]



###############################################################################
#                                     Misc                                    #
###############################################################################
@pytest.fixture
def transFlags() -> trans.TransFlags:
    return trans.TransFlags()


@pytest.fixture
def Tr() -> trans.Translator:
    return trans.Translator()


class Tr_TC:

    def test_TransFlags(self, transFlags: trans.TransFlags) -> None:
        assert transFlags.get(transFlags.ARGUMENT_AWAIT) is False

    def test_TransFlags_Recursive(self, transFlags: trans.TransFlags) -> None:

        with transFlags.recursive():
            assert transFlags.is_recursive_inner() is False
            assert transFlags.get(transFlags.ARGUMENT_AWAIT) is False
            transFlags.setTrue(transFlags.ARGUMENT_AWAIT)
            assert transFlags.get(transFlags.ARGUMENT_AWAIT) is True

        assert transFlags.get(transFlags.ARGUMENT_AWAIT) is False

    def test_Call_Expression_Transform(self, Tr, CallExpression_Cases) -> None:
        instgrp = Tr.trans(CallExpression_Cases[0])  # type: InstGrp

        assert instgrp is not None

        # Verify that insts is successful generated
        # box = BoxMachinePlus()
        assert instgrp.duts() == ["Box"]

        insts = instgrp.insts()
        assert str(insts[0]) == "query [things] __VAR__0"
        assert str(insts[1]) == "op [__VAR__0] __VAR__1"

    def test_Assign_Stmt_Transform(self, Tr, AssignStmts_Cases) -> None:
        Case_1, Case_2, Case_3 = AssignStmts_Cases

        instgrp_1 = Tr.trans(Case_1)
        #instgrp_2 = Tr.trans(Case_2)
        #instgrp_3 = Tr.trans(Case_3)

        # Verify
        assert 1 == 2

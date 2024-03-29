import DevAuto as DA
import DevAuto.Core.devCoreTypes as dtyp


@DA.function
def TC():
    GL8900T = DA.Dut("GL8900T")

    tbl = GL8900T.routeTBL()

    if tbl["127.0.0.0"]["netmask"] == "255.255.255.255":
        pass

    # This analysis is perform in python layer, and the result
    # will return to DA layer.
    # analysisResult = DA.ComplexAnalysisByRegex("a|b", result)

    # if analysisResult is True:
    #     return True
    # else:
    #     return False

    return True


@DA.function
def TC1() -> bool:
    GL8900T = DA.Dut("GL8900T")
    GL5610 = DA.Dut("GL5610")

    a = GL8900T.numOfPorts()
    b = GL5610.numOfPorts()

    if a > b:
        return True
    else:
        return False


@DA.function
def TC2() -> bool:

    # Both are DA.functions
    login()
    precheck()

    GL8900T = DA.Dut("GL8900")

    ret = GL8900T.serial("shutdown 0/1")
    ret = GL8900T.serial("port status")
    ps = core.regex("0/1", ret)

    if ps == "down":
        return True
    else:
        return False


def run_TC() -> bool:
    return run(TC())


def run_TC1() -> bool:
    return TC1()


def run_TC2() -> bool:
    return TC2()


def main() -> bool:
    return DA.tc_group([
        TC(),
        TC1(),
        TC2()
    ])

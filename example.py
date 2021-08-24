import DevAuto as DA
import DevAuto.Core.devCoreTypes as dtyp


@DA.testcase
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


def TC1():
    GL8900T = DA.Dut("GL8900T")
    GL5610 = DA.Dut("GL5610")

    a = GL8900T.numOfPorts()
    b = GL5610.numOfPorts()

    if a > b:
        return True


def TC2() -> None:
    GL8900T = DA.Dut("PC1")

    n = GL8900T.numOfPorts()
    GL8900T.port_shutdown(n)

    # such if statement need to be
    # translated, then execute in DA layer.
    if n == 2:
        pass

    m = n + 1

    if m == 3:
        pass

    return True


def main():
    pass

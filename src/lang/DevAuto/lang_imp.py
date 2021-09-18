import typing as typ


class DFunc:
    """
    DevAuto Function
    """


class DIf:
    """
    IF statement of DevAuto.
    """


class DWhile:
    """
    While statement of DevAuto
    """


def function(t: typ.Callable) -> DFunc:
    """
    Decorator that mark a python function as a DA function.
    """

import typing as typ


class DFunc:
    """
    DevAuto Function
    """


def function(t: typ.Callable) -> DFunc:
    """
    Decorator that mark a python function as a DA function.
    """

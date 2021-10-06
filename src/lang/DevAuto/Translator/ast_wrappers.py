import ast
import typing
import typing as typ



def wrap_expr_arg_proc(args: typ.List[str], node: ast.expr) -> typ.List:
    arg_proced = []

    for arg in args:
        if arg == "*":
            arg_proced.append(node)
        else:
            arg_proced.append(ast.Name(id = arg, ctx = ast.Load()))

    return arg_proced


def wrap_expr_in_func(func: str, args: typ.List[str], node: ast.expr) -> ast.expr:
    new_expr = ast.Call(
        func = ast.Name(id = func, ctx = ast.Load()),
        args = wrap_expr_arg_proc(args, node),
        keywords = []
    )
    ast.fix_missing_locations(new_expr)

    return new_expr


def function_define(func: str, args: typ.List[str],
                    body: typ.List[ast.stmt]) -> ast.FunctionDef:
    new_func = ast.FunctionDef(
        name = func,
        args = ast.arguments(
            posonlyargs = [],
            args = [],
            vararg = None,
            kwonlyargs = [],
            kw_defaults = [],
            kwarg = None,
            defaults = []
        ),
        body = body,
        decorator_list = [],
        returns = None,
        type_comment = None
    )
    ast.fix_missing_locations(new_func)

    return new_func

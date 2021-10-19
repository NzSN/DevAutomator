import ast
import astor
import copy
import inspect
import astpretty
import typing as typ
import DevAuto.transFlags as flags
import DevAuto.Core as core
import DevAuto.lang_imp as dal
import DevAuto.Translator.ast_wrappers as ast_wrapper
import DevAuto.Translator.transform as trFuncs
from DevAuto.utility import *


class Translator:

    def preprocessing_before_transform(self, tree: ast.Module, info: typ.List) -> ast.AST:
        funcdef = tree.body[0]
        assert(isinstance(funcdef, ast.FunctionDef))

        # Remove decorator
        funcdef.decorator_list = []

        return tree

    def preprocessing_after_transform(self, tree: ast.Module, info: typ.List) -> ast.AST:

        funName = info[0]

        # Add Insts :: List[Inst] as an arg
        # Insts is used to hold all Insts that equal
        # to the dal.DFunc.
        funcdef = tree.body[0]
        assert(isinstance(funcdef, ast.FunctionDef))
        funcdef.args.args.append(ast.arg(arg="insts", annotation=None, type_comment=None))

        # Append a call expr
        # without this expr the pyfunc will not be executed
        # in exec()
        expr = ast.Expr(
            ast.Call(
                func = ast.Name(id=funName, ctx=ast.Load()),
                args = [ ast.Name(id='insts', ctx = ast.Load()) ],
                keywords = []))
        tree.body.append(expr)

        ast.fix_missing_locations(tree)

        return tree

    def environmentInit(self, func: dal.DFunc) -> typ.Dict:
        env = func.env()
        env["insts"] = dal.InstGrp([], [], [])
        env["da_expr_convert"] = trFuncs.da_expr_convert
        env["da_unwrap"] = trFuncs.da_unwrap
        env["da_as_arg"] = trFuncs.da_as_arg
        env["da_as_assign_value"] = trFuncs.da_as_assign_value

        return env

    def trans(self, func: dal.DFunc) -> dal.InstGrp:
        """
        Transform a dal.DFunc into list of Insts.
        """
        pyfunc = func.body()
        global_env = self.environmentInit(func)

        # Transform AST nodes
        ast_nodes = ast.parse(inspect.getsource(pyfunc))
        self.preprocessing_before_transform(ast_nodes, [pyfunc.__name__])

        DA_NodeTransformer(global_env).visit(ast_nodes)

        self.preprocessing_after_transform(ast_nodes, [pyfunc.__name__])

        # TODO: Remove ast tree print
        print(astor.to_source(ast_nodes))

        # Transform from ast to List[Inst]
        exec(compile(ast_nodes, "", 'exec'), global_env, {})

        return global_env["insts"]


class DA_NodeTransformer(ast.NodeTransformer):
    """
    Transform a dal.DFunc into intermidiate form which able
    to be execute by python interpreter to generate a list
    of DA instructions.
    """

    def __init__(self, env: typ.Dict) -> None:
        self._env = env
        self._precheck_transformer = DA_NodeTransPreCheck(env)
        self._trans_transformer = DA_NodeTransTransform(env)

    def visit(self, node: ast.AST) -> None:
        """
        Make transformations to dal.DFunc's ast nodes
        """

        # Make sure the ast is able to be transformed
        self._precheck_transformer.visit(node)

        # Do Transformation
        self._trans_transformer.visit(node)


class DA_NodeTransPreCheck(ast.NodeTransformer):
    """
    Do prechecking to ast nodes of dal.DFunc
    """

    def __init__(self, env: typ.Dict) -> None:
        ast.NodeTransformer.__init__(self)
        self._env = env


class TransFlags:

    ARGUMENT_AWAIT = "arg"
    IF_TEST_EXPR = "ITE"
    ASSIGNMENT_STMT = "AS"

    def __init__(self) -> None:
        self._flagsDefault = {
            # Indicate that current expr is
            # argument of another expr
            self.ARGUMENT_AWAIT: False,

            self.IF_TEST_EXPR: None,
            self.ASSIGNMENT_STMT: False
        }  # type: typ.Dict[str, typ.Any]

        self._stack = [copy.deepcopy(self._flagsDefault)]

    def __enter__(self) -> None:
        self._recursive_in()

    def __exit__(self, exc_type, exc_value, traceback) -> None:
        self._recursive_exit()

    def current(self) -> typ.Dict[str, typ.Any]:
        return self._stack[-1]

    def get(self, key: str) -> typ.Any:
        current = self.current()

        if key in current:
            return current[key]
        else:
            # TODO: Provide a more concrete exception
            raise Exception("Key not found")

    def set(self, key: str, value: typ.Any) -> None:
        self.current()[key] = value

    def unset(self, key: str) -> None:
        self.current()[key] = None

    def setTrue(self, key: str) -> None:
        self.current()[key] = True

    def setFalse(self, key: str) -> None:
        self.current()[key] = False

    def arg_await(self) -> None:
        """
        Set arg_await flag to let Transformer to
        provide subexpr's value to parent.
        """
        self.setTrue(self.ARGUMENT_AWAIT)

    def in_assign_proc(self) -> None:
        self.setTrue(self.ASSIGNMENT_STMT)

    def is_in_assign_proc(self) -> bool:
        return self.get(self.ASSIGNMENT_STMT) == True

    def is_arg_await(self) -> bool:
        return self.current()[self.ARGUMENT_AWAIT] == True

    def is_if_test(self) -> bool:
        return self.current()[self.IF_TEST_EXPR] == True

    def recursive(self) -> 'TransFlags':
        return self

    def _recursive_in(self) -> None:
        self._stack.append(copy.deepcopy(self._flagsDefault))

    def _recursive_exit(self) -> None:
        if len(self._stack) > 1:
            self._stack.pop()
        else:
            return

    def is_recursive_inner(self) -> bool:
        return len(self._stack) == 0


class DA_NodeTransTransform(ast.NodeTransformer):
    """
    Do transfromations to ast nodes of dal.DFunc
    """

    def __init__(self, env: typ.Dict) -> None:
        ast.NodeTransformer.__init__(self)
        self._env = env
        self._func_ident_gen = IdentGenerator("funcGen", "_lambda", 10000)
        self._flags = TransFlags()

    def decorate(self, node: ast.AST) -> typ.Optional[ast.AST]:

        new = None

        if self._flags.is_arg_await():
            # New insts is as argument of another inst.
            new = ast_wrapper.call(
                ast.Name(id="da_as_arg", ctx=ast.Load()),
                [ast.Name(id="insts", ctx=ast.Load()),
                 typ.cast(ast.expr, node)])

        elif self._flags.is_if_test():
            # New insts is as condition expression of
            # an Jmpxx inst.
            ...
        elif self._flags.is_in_assign_proc():
            new = ast_wrapper.call(
                ast.Name(id="da_as_assign_value", ctx=ast.Load()),
                [ast.Name(id="insts", ctx=ast.Load()),
                 typ.cast(ast.expr, node)]
            )
        else:
            return node

        # Unwrap snippet to provide it's value to
        # another expression.
        new = ast_wrapper.call(
            ast.Name(id="da_unwrap", ctx=ast.Load()),
            [typ.cast(ast.expr, new)]
        )

        return new

    def visit_For(self, node):
        return node

    def visit_While(self, node):
        return node

    def visit_If(self, node: ast.If):

        # Get dynamic identifier for body function and
        # elseBody function to prevent global namespace
        # conflicts
        body_func_id = self._func_ident_gen.gen()
        else_body_func_id = self._func_ident_gen.gen()

        # Wrap body of if stmt into functions
        bodyDef = ast_wrapper.function_define_posonly(
            body_func_id, [], node.body)
        elseBodyDef = ast_wrapper.function_define_posonly(
            else_body_func_id, [], node.orelse)

        # Transform stmts in body and elsebody recursively.
        self.visit(bodyDef)
        self.visit(elseBodyDef)

        ifCalling = ast_wrapper.call(
            ast_wrapper.call(
                ast.Name("DIf", ctx=ast.Load()),
                [ast.Name(id="insts", ctx=ast.Load()),
                 node.test,
                 ast.Name(id=body_func_id, ctx=ast.Load()),
                 ast.Name(id=else_body_func_id, ctx=ast.Load())]
            ),
            [ast.Name(id="da_if_convert", ctx=ast.Load())]
        )
        ast.fix_missing_locations(ifCalling)

        self._env['DIf'] = dal.DIf
        self._env['da_if_convert'] = trFuncs.da_if_convert

        return [bodyDef, elseBodyDef, ifCalling]

    def visit_Return(self, node):
        return node

    def visit_BinOp(self, node):
        return node

    def visit_Assign(self, node: ast.Assign) -> typ.List[ast.AST]:
        if len(node.targets) > 1:
            raise Exception("Multiple assign is not supported")

        target_nodes = None
        value_nodes = None

        # Transform target and value
        with self._flags.recursive():

            # Switch into assign proc mode
            self._flags.in_assign_proc()

            target_nodes = self.visit(node.targets[0])
            value_nodes = self.visit(node.value)

        if target_nodes is None or \
           value_nodes is None:
            return [node]

        assign = typ.cast(ast.Assign, ast_wrapper.parse_stmt("v = 0"))
        assign.targets = [target_nodes]
        assign.value = value_nodes

        target_load = copy.deepcopy(target_nodes)
        target_load.ctx = ast.Load()
        target_ident = target_load.id

        call_expr = ast.Expr(value=ast_wrapper.call(
            ast.Name(id="da_assign_transform", ctx=ast.Load()),
            [ast.Name(id="insts", ctx=ast.Load()),
             ast.Constant(value=target_ident),
             target_load]))

        # Environment updates
        self._env["da_assign_transform"] = trFuncs.da_assign_transform

        return [assign, call_expr]

    def visit_Constant(self, node: ast.Constant) -> ast.Constant:
        return node

    def visit_Call(self, node: ast.Call) -> ast.Call:

        args = []

        if len(node.args) > 0:

            # Transform every argument of this call
            # into intermidiate form
            with self._flags.recursive():
                self._flags.arg_await()
                args = [self.visit(arg) for arg in node.args]

            # Replace original arguments
            # with transformed arguments.
            node.args = args

        # To check that is any Operations is called
        # as arguments of this calling.
        transform_node = typ.cast(ast.Call, ast_wrapper.parse_expr(
            "da_call_transform(insts)"))
        transform_node.args.append(node)

        decorated_node = self.decorate(transform_node)
        if decorated_node is None:
            # TODO: Should provide a more precise exception.
            raise Exception("Failed to decorate a call expression")

        self._env["da_call_transform"] = trFuncs.da_call_transform
        return typ.cast(ast.Call, decorated_node)

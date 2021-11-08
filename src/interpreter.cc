#include "interpreter.hpp"
#include "utility.hpp"
#include <memory>

namespace fs = std::filesystem;
using std::unique_ptr;

namespace Interpreter_Internal {
    void * PyInstsToNativeInsts(PyObject_ptr &PyInsts) {}
}

Job Interpreter::interpret(TestCase &tc) {
    // Evaluate TestCase
    PyObject_ptr PyInsts = pyFuncEvaluate(fs::path(tc.path()), "main");

    // Transform PyInsts into C++-Insts
    void *insts = Interpreter_Internal::PyInstsToNativeInsts(PyInsts);

    return {};
}

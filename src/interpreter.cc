#include "interpreter.hpp"
#include "utility.hpp"
#include <memory>
#include <exception>

namespace fs = std::filesystem;
using std::unique_ptr;

namespace Interpreter_Internal {
    void * PyInstsToNativeInsts(PyObject_ptr &PyInsts) {}
}

Job Interpreter::interpret(TestCase &tc) {
    // Evaluate TestCase
    // Caution: modulePath should be configured by user.
    std::optional<PyObject_ptr> PyInstsMaybe = pyFuncEvaluate(fs::path(tc.path()), L"");
    if (!PyInstsMaybe.has_value()) {
        throw std::runtime_error("Failed to eval python function");
    }

    // Transform PyInsts into C++-Insts
    void *insts = Interpreter_Internal::PyInstsToNativeInsts(PyInstsMaybe.value());

    return {};
}

#include "interpreter.hpp"
#include "utility.hpp"
#include <memory>

namespace fs = std::filesystem;
using std::unique_ptr;

namespace {
    void * PyInstsToNativeInsts(unique_ptr<PyObject> &PyInsts) {

    }
}

Job Interpreter::interpret(TestCase &tc) {
    // Evaluate TestCase
    unique_ptr<PyObject> PyInsts = pyFuncEvaluate(fs::path(tc.path()));

    // Transform PyInsts into C++-Insts
    void *insts = PyInstsToNativeInsts(PyInsts);
}

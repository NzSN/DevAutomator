#include "interpreter.hpp"
#include "utility.hpp"
#include <memory>
#include <exception>
#include <optional>
#include "DAL/cond_insts.hpp"
#include "DAL/var_insts.hpp"
#include "DAL/oper_insts.hpp"

namespace fs = std::filesystem;
using std::unique_ptr, std::shared_ptr;


namespace Interpreter_Internal {

    shared_ptr<Instruction> instTrans(PyObject *PyInst) {
        int opcode = PyLong_AsLong(
            PyObject_GetAttr(PyInst, PyUnicode_FromString("_inst_code_int")));

        switch (opcode) {
        case JMP_INST: {
            return std::make_shared<JMP>(PyInst);
        }
        case JMP_TRUE_INST: {
            return std::make_shared<JMP_TRUE>(PyInst);
        }
        case JMP_FALSE_INST: {
            return std::make_shared<JMP_FALSE>(PyInst);
        }
        case DEF_INST : {
            return std::make_shared<DEF>(PyInst);
        }
        case EQUAL_INST : {
            return std::make_shared<EQUAL>(PyInst);
        }
        case OPER_INST : {
            return std::make_shared<Oper>(PyInst);
        }
        }
        return NULL;
    }

    InstructionSet PyInstsToNativeInsts(PyObject_ptr &PyInsts) {
        InstructionSet set;

        // type(PyObject_ptr) should be 'InstGrp'
        bool isInstGrp = PyObject_HasAttr(
            PyInsts.get(), PyUnicode_FromString("_insts"));

        if (!isInstGrp) {
            // Error
            throw std::runtime_error(
                "PyINstsToNativeInsts: Not an InstGrp");
        }

        PyObject *insts = PyObject_GetAttr(
            PyInsts.get(), PyUnicode_FromString("_insts"));

        Py_ssize_t size = PyList_Size(insts);
        for (Py_ssize_t i = 0; i < size; ++i) {
            PyObject *PyInst = PyList_GetItem(insts, i);
            set.append(instTrans(PyInst));
        }

        return set;
    }
}


InstructionSet Interpreter::interpret(TestCase &tc) {
    // Evaluate TestCase
    // Caution: modulePath should be configured by user.
    std::optional<PyObject_ptr> PyInstsMaybe =
        pyFuncEvaluate(fs::path(tc.path()));
    if (!PyInstsMaybe.has_value()) {
        throw std::runtime_error("Failed to eval python function");
    }

    // Transform PyInsts into C++-Insts
    return Interpreter_Internal::
        PyInstsToNativeInsts(PyInstsMaybe.value());
}

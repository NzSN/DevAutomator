#include <string>
#include "DAL/instruction.hpp"
#include "DAL/cond_insts.hpp"
#include "DAL/var_insts.hpp"
#include "DAL/oper_insts.hpp"


using std::string, std::shared_ptr, std::make_shared;


///////////////////////////////////////////////////////////////////////////////
//                       InstructionSet Implementation                       //
///////////////////////////////////////////////////////////////////////////////
void InstructionSet::eval(DAL_Environment &env) {
    for (auto &inst: insts)
        inst->eval(env);
}


///////////////////////////////////////////////////////////////////////////////
//                    Variable Instruction Implementation                    //
///////////////////////////////////////////////////////////////////////////////
DEF::DEF(PyObject *PyDef): VARInst(DEF_INST) {
    ident = PyUnicode_AsUTF8(
        PyObject_GetAttr(PyDef, PyUnicode_FromString("_ident")));
    varValue = PyUnicode_AsUTF8(
        PyObject_GetAttr(PyDef, PyUnicode_FromString("_value")));
}

void DEF::eval(DAL_Environment &evn) {}


void EQUAL::eval(DAL_Environment &env) {}


EQUAL::EQUAL(PyObject *PyEqual): VARInst(EQUAL_INST) {
    string varIdent;

    PyObject *PyLeft = PyObject_GetAttr(PyEqual, PyUnicode_FromString("_l"));
    PyObject *PyRight = PyObject_GetAttr(PyEqual, PyUnicode_FromString("_r"));

    // To check that argument of equal is Literal or Var
    if (PyUnicode_Check(PyLeft)) {
        leftTerm = make_shared<Literal>(PyUnicode_AsUTF8(PyLeft));
    } else {
        varIdent = PyUnicode_AsUTF8(
            PyObject_GetAttr(PyLeft, PyUnicode_FromString("ident")));
        leftTerm = make_shared<VAR>(varIdent);
    }

    if (PyUnicode_Check(PyRight)) {
        rightTerm = make_shared<Literal>(PyUnicode_AsUTF8(PyRight));
    } else {
        varIdent = PyUnicode_AsUTF8(
            PyObject_GetAttr(PyRight, PyUnicode_FromString("ident")));
        rightTerm = make_shared<VAR>(varIdent);
    }
}

///////////////////////////////////////////////////////////////////////////////
//                    Condition Instruction Implementation                   //
///////////////////////////////////////////////////////////////////////////////

namespace {
    void JMP_XXX_INIT(PyObject *PyJmp, shared_ptr<TERM> &cond) {
        PyObject *test = PyObject_GetAttr(
            PyJmp, PyUnicode_FromString("_test"));

        if (PyBool_Check(test)) {
            if (test == Py_True) {
                cond = make_shared<TRUE_>();
            } else {
                cond = make_shared<FALSE_>();
            }
        } else {
            string ident = PyUnicode_AsUTF8(
                PyObject_GetAttr(test, PyUnicode_FromString("ident")));
            cond = make_shared<VAR>(ident);
        }
    }
}


void JMP::eval(DAL_Environment &env) {}

JMP_TRUE::JMP_TRUE(PyObject *PyJmp): JMP(PyJmp) {
    JMP_XXX_INIT(PyJmp, cond);
}


JMP_FALSE::JMP_FALSE(PyObject *PyJmp) : JMP(PyJmp) {
    JMP_XXX_INIT(PyJmp, cond);
}


///////////////////////////////////////////////////////////////////////////////
//                    Operation Instruction Implementation                   //
///////////////////////////////////////////////////////////////////////////////
void Oper::eval(DAL_Environment &env) {}

Oper::Oper(PyObject *PyOper): OPERInst(OPER_INST) {
    opcode = PyUnicode_AsUTF8(
        PyObject_GetAttr(PyOper, PyUnicode_FromString("_opcode")));
    src = PyUnicode_AsUTF8(
        PyObject_GetAttr(PyOper, PyUnicode_FromString("_src")));
    dst = PyUnicode_AsUTF8(
        PyObject_GetAttr(PyOper, PyUnicode_FromString("_dst")));
}


///////////////////////////////////////////////////////////////////////////////
//                                    TERM                                   //
///////////////////////////////////////////////////////////////////////////////
void VAR::eval(DAL_Environment &env) {

}


void Literal::eval(DAL_Environment &env) {}


void TRUE_::eval(DAL_Environment &env) {}


void FALSE_::eval(DAL_Environment &env) {}

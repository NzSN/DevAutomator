#include <string>
#include "DAL/instruction.hpp"
#include "DAL/cond_insts.hpp"
#include "DAL/var_insts.hpp"
#include "DAL/oper_insts.hpp"


using std::string;


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

void DEF::eval(DAL_Environment &evn) {

}


void EQUAL::eval(DAL_Environment &env) {}

///////////////////////////////////////////////////////////////////////////////
//                    Condition Instruction Implementation                   //
///////////////////////////////////////////////////////////////////////////////
void JMP::eval(DAL_Environment &env) {}



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

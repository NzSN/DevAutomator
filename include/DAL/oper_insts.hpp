#include "general.hpp"
#include <Python.h>
#include <string>
#include "DAL/instruction.hpp"
#include "DAL/environment.hpp"


using std::string;

#ifndef OPER_INSTS_H
#define OPER_INSTS_H

class Oper: public OPERInst {
public:
    Oper(string opcode_, string src_, string dst_) :
        OPERInst(OPER_INST), opcode(opcode_), src(src_), dst(dst_) {}
    Oper(PyObject *PyOper);
    ~Oper() {}

    string getOpCode() {
        return opcode;
    }

    string getSrc() {
        return src;
    }

    string getDst() {
        return dst;
    }

    void eval(DAL_Environment &env);

  private:
    string opcode;
    string src;
    string dst;
};

#endif /* OPER_INSTS_H */

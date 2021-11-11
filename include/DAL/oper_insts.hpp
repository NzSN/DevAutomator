#include "general.hpp"
#include <Python.h>
#include <string>
#include "DAL/instruction.hpp"
#include "DAL/environment.hpp"
#include <memory>


using std::string;

#ifndef OPER_INSTS_H
#define OPER_INSTS_H

using ArgsPtr = std::shared_ptr<std::vector<string>>;

class Oper: public OPERInst {
public:
    Oper(string opcode_, string src_, string dst_, ArgsPtr args_) :
        OPERInst(OPER_INST), opcode(opcode_), src(src_), dst(dst_), args(args_) {}
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

    ArgsPtr getArgs() {
        return args;
    }

    void eval(DAL_Environment &env);

  private:
    string opcode;
    string src;
    string dst;
    ArgsPtr args;
};

#endif /* OPER_INSTS_H */

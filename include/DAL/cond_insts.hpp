#include "general.hpp"
#include "DAL/instruction.hpp"
#include "environment.hpp"
#include <Python.h>
#include <functional>
#include <memory>


#ifndef COND_INSTS_H
#define COND_INSTS_H

class JMP : public CONDInst {
    /**
     * JMP Instruction used to set which
     * Instruction to be runned next.
     */
public:
    JMP(int idx) : CONDInst(JMP_INST),
                   toIdx(idx) {}
    JMP(PyObject *PyJmp) :
        CONDInst(JMP_INST),
        toIdx(PyLong_AsLong(PyObject_GetAttr(
              PyJmp, PyUnicode_FromString("_to")))) {}
    ~JMP() {}
    int where() const {
        return toIdx;
    }

    void eval(DAL_Environment &env);
protected:
    int toIdx;
};


class JMP_TRUE: public JMP {
    /**
     * JMP Instruct used to set which
     * Instruction to be runned next
     * if the condition is true.
     */
public:
    JMP_TRUE(int idx, std::shared_ptr<TERM> cond_) :
        JMP(idx), cond(cond_) {
        toIdx = JMP_TRUE_INST;
    }
    JMP_TRUE(PyObject *PyJmp);
    std::shared_ptr<TERM> condition() {
        return cond;
    }
private:
    std::shared_ptr<TERM> cond;
};


class JMP_FALSE: public JMP {
    /**
     * JMP Instruct used to set which
     * Instruction to be runned next
     * if the condition is false.
     */
public:
    JMP_FALSE(int idx, std::shared_ptr<TERM> cond_) :
        JMP(idx), cond(cond_) {
        toIdx = JMP_FALSE_INST;
    }
    JMP_FALSE(PyObject *PyJmp);

    std::shared_ptr<TERM> condition() {
        return cond;
    }
private:
    std::shared_ptr<TERM> cond;
};


#endif /* COND_INSTS_H */

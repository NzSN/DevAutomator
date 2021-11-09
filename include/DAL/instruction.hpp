#include "general.hpp"
#include <vector>
#include <iterator>
#include <memory>
#include "DAL/environment.hpp"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum {
    JMP_INST = 0,
    JMP_TRUE_INST,
    JMP_FALSE_INST,
    DEF_INST,
    EQUAL_INST,
} InstCode;

class Instruction {
public:
    Instruction(InstCode iCode_) : iCode(iCode_) {}
    virtual ~Instruction() = 0;
    int code() const {
        return iCode;
    }
    virtual void eval(DAL_Environment&) = 0;
protected:
    // Instruction Code it's identifer of
    // Instructions.
    InstCode iCode;
};

using InstPtr = std::shared_ptr<Instruction>;


class InstructionSet {
public:
    auto begin() {
        return insts.begin();
    }

    auto end() {
        return insts.end();
    }

    void append(InstPtr inst) {
        insts.push_back(inst);
    }

    void eval(DAL_Environment &env);

private:
    std::vector<InstPtr> insts;
};


class CONDInst : public Instruction {
public:
  CONDInst(InstCode iCode_) : Instruction(iCode_) {}
};

class OPERInst: public Instruction {
public:
    OPERInst(InstCode iCode_) : Instruction(iCode_) {}
};


class VARInst: public Instruction {
public:
    VARInst(InstCode iCode_) : Instruction(iCode_) {}
};

class TERM {};

class VAR : TERM {
};

class Literal : TERM {};

#endif /* INSTRUCTION_H */

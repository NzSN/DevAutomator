#include "general.hpp"

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum {
    JMP_INST = 0,
    JMP_TRUE_INST,
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
protected:
    // Instruction Code it's identifer of
    // Instructions.
    InstCode iCode;
};


class CONDInst: public Instruction {
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

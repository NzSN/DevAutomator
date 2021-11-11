#include "general.hpp"
#include <vector>
#include <iterator>
#include <memory>
#include "DAL/environment.hpp"
#include <string>

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef enum {
    JMP_INST = 0,
    JMP_TRUE_INST,
    JMP_FALSE_INST,
    DEF_INST,
    EQUAL_INST,
    OPER_INST,
    VAR_INST,
    LITERAL_INST,
    TRUE_INST,
    FALSE_INST,
} InstCode;

class Instruction {
public:
    Instruction() = default;
    Instruction(InstCode iCode_) : iCode(iCode_) {}
    virtual ~Instruction() {};
    int code() const {
        return iCode;
    }
    virtual void eval(DAL_Environment &){};

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

    size_t size() {
        return insts.size();
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


class TERM: public Instruction {
public:
    TERM(InstCode iCode_) : Instruction(iCode_) {}
};


class VAR : public TERM {
public:
    VAR(std::string ident_) : TERM(VAR_INST), ident(ident_) {}
    ~VAR() {}
    void eval(DAL_Environment &env);

private:
    std::string ident;
};


class Literal : public TERM {
public:
    Literal(std::string value_) :
        TERM(LITERAL_INST), value(value_) {}
    ~Literal() {}
    void eval(DAL_Environment &env);

private:
    std::string value;
};


class TRUE_ : public TERM {
public:
    TRUE_() : TERM(TRUE_INST) {}
    ~TRUE_() {}
    void eval(DAL_Environment &env);
};


class FALSE_ : public TERM {
public:
    FALSE_() : TERM(FALSE_INST) {}
    ~FALSE_() {}
    void eval(DAL_Environment &env);
};

#endif /* INSTRUCTION_H */

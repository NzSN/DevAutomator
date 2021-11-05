#include "general.hpp"
#include "DAL/instruction.hpp"
#include <functional>

using std::function;


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
    int where() const {
        return toIdx;
    }
private:
    int toIdx;
};


class JMP_TRUE: public JMP {
    /**
     * JMP Instruct used to set which
     * Instruction to be runned next
     * if the condition is true.
     */
public:
    JMP_TRUE(int idx, TERM cond_) :
        JMP(idx), cond(cond_) {}
    TERM condition() {
        return cond;
    }
private:
    TERM cond;
};


class JMP_FALSE: public JMP {
    /**
     * JMP Instruct used to set which
     * Instruction to be runned next
     * if the condition is false.
     */
public:
    JMP_FALSE(int idx, TERM cond_) :
        JMP(idx), cond(cond_) {}
    TERM condition() {
        return cond;
    }
private:
    TERM cond;
};


#endif /* COND_INSTS_H */

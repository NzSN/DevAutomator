#include "general.hpp"
#include "DAL/instruction.hpp"
#include <string>

using std::string;


#ifndef VAR_INSTS_H
#define VAR_INSTS_H


class DEF : VARInst {
public:
    DEF(string ident_, string value_) :
        VARInst(DEF_INST), ident(ident_), varValue(value_) {}
    string identifier() {
        return ident;
    }

    string value() {
        return varValue;
    }

private:
    string ident;
    string varValue;
};


class EQUAL : VARInst {
public:
    EQUAL(TERM left, TERM right) :
        VARInst(EQUAL_INST), leftTerm(left), rightTerm(right) {}

    TERM left() {
        return leftTerm;
    }

    TERM right() {
        return rightTerm;
    }
private:
    TERM leftTerm;
    TERM rightTerm;
};


#endif /* VAR_INSTS_H */

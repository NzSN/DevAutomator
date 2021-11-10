#include "general.hpp"
#include <Python.h>
#include "DAL/instruction.hpp"
#include "DAL/environment.hpp"
#include <string>

using std::string;


#ifndef VAR_INSTS_H
#define VAR_INSTS_H


class DEF : public VARInst {
public:
    DEF(string ident_, string value_) :
        VARInst(DEF_INST), ident(ident_), varValue(value_) {}
    DEF(PyObject *PyDef);

    string identifier() {
        return ident;
    }

    string value() {
        return varValue;
    }

    void eval(DAL_Environment &env);

private:
    string ident;
    string varValue;
};


class EQUAL : public VARInst {
public:
    EQUAL(TERM left, TERM right) :
        VARInst(EQUAL_INST), leftTerm(left), rightTerm(right) {}
    EQUAL(PyObject *PyEqual):
        VARInst(EQUAL_INST) {}
    ~EQUAL() {}

    void eval(DAL_Environment &);

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

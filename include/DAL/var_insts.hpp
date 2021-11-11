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
    EQUAL(std::shared_ptr<TERM> left, std::shared_ptr<TERM> right) :
        VARInst(EQUAL_INST), leftTerm(left), rightTerm(right) {}
    EQUAL(PyObject *PyEqual);
    ~EQUAL() {}

    void eval(DAL_Environment &);

    std::shared_ptr<TERM> left() {
        return leftTerm;
    }

    std::shared_ptr<TERM> right() {
        return rightTerm;
    }

  private:
    std::shared_ptr<TERM> leftTerm;
    std::shared_ptr<TERM> rightTerm;
};


#endif /* VAR_INSTS_H */

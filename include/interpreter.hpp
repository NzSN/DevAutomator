#include "general.hpp"
#include "testCase.hpp"
#include "job.hpp"


#ifndef INTERPRETER_H
#define INTERPRETER_H

class Interpreter {
public:
    Job interpret(TestCase &tc);
};

#endif /* INTERPRETER_H */

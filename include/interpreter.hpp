#include "general.hpp"
#include "testCase.hpp"
#include "DAL/instruction.hpp"
#include <filesystem>


#ifndef INTERPRETER_H
#define INTERPRETER_H

class Interpreter {
public:
    InstructionSet interpret(TestCase &tc, std::filesystem::path transPath);
};

#endif /* INTERPRETER_H */

#include "general.hpp"
#include "testCase.hpp"
#include "DAL/instruction.hpp"
#include <Python.h>
#include <filesystem>
#include <string>


#ifndef INTERPRETER_H
#define INTERPRETER_H

class Interpreter {
public:
    Interpreter(std::filesystem::path modPath): searchPaths(Py_GetPath()) {
        searchPaths += L":" + modPath.wstring();
        // Setup Modules search paths
        Py_SetPath(searchPaths.wstring().c_str());
        Py_Initialize();
    }

    ~Interpreter() {
        // Before that you should make
        // sure all PyObjects is decrefed.
        Py_FinalizeEx();
    }

    InstructionSet interpret(TestCase &tc);

private:
    std::filesystem::path searchPaths;
};

#endif /* INTERPRETER_H */

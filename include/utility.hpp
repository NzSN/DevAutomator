#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>
#include <optional>

#ifndef UTILITY_H
#define UTILITY_H

using PyObject_ptr = std::unique_ptr<PyObject, void(*)(PyObject*)>;

// TODO: Need to move this function into Interpreter
//       to lock all Python-Interpreter related codes
//       into it.
std::optional<PyObject_ptr>
pyFuncEvaluate(std::filesystem::path p, std::string func = "main");

#endif /* UTILITY_H */

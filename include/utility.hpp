#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>

#ifndef UTILITY_H
#define UTILITY_H

using PyObject_ptr = std::unique_ptr<PyObject, void(*)(PyObject*)>;

PyObject_ptr pyFuncEvaluate(std::filesystem::path p, std::string func = "main");

#endif /* UTILITY_H */

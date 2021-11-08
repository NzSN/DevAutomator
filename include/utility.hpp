#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>
#include <optional>

#ifndef UTILITY_H
#define UTILITY_H

using PyObject_ptr = std::unique_ptr<PyObject, void(*)(PyObject*)>;

std::optional<PyObject_ptr>
pyFuncEvaluate(std::filesystem::path p, std::wstring modulePath, std::string func = "main");

#endif /* UTILITY_H */

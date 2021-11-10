#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>
#include "utility.hpp"
#include <optional>
#include "DAL/instruction.hpp"
#include <iostream>


using std::string, std::wstring, std::filesystem::path;
using std::optional;


void PyObjDeleter(PyObject *obj) {
    Py_DECREF(obj);
 }


/**
 * This function should only called after Py_Initialize() and before Py_Finalize()
 */
std::optional<PyObject_ptr> pyFuncEvaluate(path p, string func) {
    PyObject *pValue = NULL, *pFunc = NULL;

    // Get Python module name
    string fileName = p.filename().string();
    string modName = fileName.substr(0, fileName.find("."));

    PyObject *pName = PyUnicode_DecodeFSDefault(modName.c_str());
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, func.c_str());

        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, NULL);
            if (pValue != NULL) {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                return {{ pValue, PyObjDeleter }};
            }
        } else {
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);

            return std::nullopt;
        }
    }

    return std::nullopt;
}

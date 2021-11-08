#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>
#include "utility.hpp"
#include <optional>


using std::string, std::filesystem::path;
using std::optional;


void PyObjDeleter(PyObject *obj) {
    Py_DECREF(obj);
 }


std::optional<PyObject_ptr> pyFuncEvaluate(path p, string func) {
    PyObject *pValue = NULL, *pFunc = NULL;

    Py_Initialize();

    // Add parent path of TestCase into PATH otherwise
    // Python Interpreter unable to load the module.
    PyRun_SimpleString("import sys");
    string modPath = "sys.path.append('" + p.parent_path().string() + "')";
    PyRun_SimpleString(modPath.c_str());

    PyObject *pName = PyUnicode_DecodeFSDefault(p.filename().c_str());
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, func.c_str());

        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, NULL);
            if (pValue != NULL) {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                Py_Finalize();
                return {{ pValue, PyObjDeleter }};
            }
        } else {
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);

            Py_Finalize();
            return std::nullopt;
        }
    }

    Py_Finalize();

    return std::nullopt;
}

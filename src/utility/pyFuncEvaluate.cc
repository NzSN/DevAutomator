#include <Python.h>
#include <string>
#include <filesystem>
#include <memory>
#include "utility.hpp"


using std::string, std::filesystem::path;


void PyObjDeleter(PyObject *obj) {
    Py_DECREF(obj);
 }


PyObject_ptr pyFuncEvaluate(path p, string func) {
    PyObject *pValue = NULL, *pFunc = NULL;

    Py_Initialize();

    // Add parent path of TestCase into PATH otherwise
    // Python Interpreter unable to load the module.
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(("sys.path.append(" + p.parent_path().string() + ")").c_str());

    PyObject *pName = PyUnicode_DecodeFSDefault(p.filename().c_str());
    PyObject *pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule, func.c_str());

        if (pFunc && PyCallable_Check(pFunc)) {
            pValue = PyObject_CallObject(pFunc, NULL);
        }
    }

    Py_DECREF(pName);
    Py_DECREF(pModule);
    Py_DECREF(pFunc);

    return {pValue, PyObjDeleter};

}

#include "gtest/gtest.h"
#include <string>
#include <filesystem>
#include <fstream>
#include "utility.hpp"
#include <Python.h>

namespace fs = std::filesystem;
using std::string, std::ofstream;


class PyFuncEvalTest_Fixture: public ::testing::Test {
    /**
     * This fixture create a directory which contain
     * a python module named "m" that consist of functions:
     * 1. def f()
     * 2. def g()
     */
protected:
    void SetUp() override {
        // Create directory
        fs::create_directory(path);

        // Create PythonModule
        ofstream mod {path + "/" + modName + ".py"};

        // Create function f
        mod <<
            "def f():\n"
            "    return 0\n"
            "def g():\n"
            "    return 1\n"
            "def main():\n"
            "    return f() + g()";

        mod.close();

        mod.open(path + "/" + "__init__.py");
        mod.close();
    }

    void TearDown() override {
        fs::remove_all(path);
    }


    string path = "/tmp/PyfFuncEvalTest_Fixture/";
    string modName = "m";
};


TEST_F(PyFuncEvalTest_Fixture, Eval) {
    std::optional<PyObject_ptr> objPtr = pyFuncEvaluate(path+"/"+modName);
    EXPECT_NE(objPtr.has_value(), NULL);

    PyObject_ptr &ptr = objPtr.value();
    int value = PyLong_AsLong(&(*ptr));
    EXPECT_EQ(value, 1);
}

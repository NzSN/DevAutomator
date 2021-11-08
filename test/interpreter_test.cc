#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include "utility.hpp"
#include <memory>

namespace fs = std::filesystem;
using std::string, std::ofstream, std::unique_ptr;

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
        ofstream mod {path + "/" + modName};

        // Create function f
        mod <<
            "def f():\n"
            "    return 0\n"
            "def g():\n"
            "    return 1\n";
        mod.close();
    }

    void TearDown() override {
        //fs::remove_all(path);
    }


    string path = "/tmp/PyfFuncEvalTest_Fixture";
    string modName = "m";
};


TEST(PyFuncEvalTest_Fixture, Evaluate) {

}

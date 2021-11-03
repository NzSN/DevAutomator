#include "general.h"
#include <fstream>
#include <string>

using std::string;

#ifndef TESTCASE_H
#define TESTCASE_H

class TestCase {
private:
    string name;
    string type;
    // Path of TestCase on local
    // file system
    string path;
};

#endif /* TESTCASE_H */

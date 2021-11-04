#include "general.h"
#include <fstream>
#include <string>
#include <iostream>

using std::string;

#ifndef TESTCASE_H
#define TESTCASE_H

class TestCase {
public:
    TestCase() = default;
    TestCase(string name, string group, string path):
        name_(name), group_(group), path_(path) {}
    TestCase(string name, string group):
        name_(name), group_(group) {}
    TestCase(string name) : name_(name) {}

    string name() const {
        return name_;
    };
    string group() const {
        return group_;
    };
    string path() const {
        return path_;
    };
private:
    string name_;
    string group_;
    // Path of TestCase on local
    // file system
    string path_;
};

#endif /* TESTCASE_H */

#include "tcdb_drivers.h"
#include <filesystem>
#include <exception>

namespace fs = std::filesystem;
using std::vector;


TCDB_ALLOCATOR tcdbDriverAllocator = {
    {"git", [](string addr, string path) -> unique_ptr<TCDB_Driver> {
            return std::make_unique<TCDB_GitDriver>(addr, path); }},
    {"local", [](string addr, string path) -> unique_ptr<TCDB_Driver> {
            return std::make_unique<TCDB_LocalDriver>(addr, path); }},
};

///////////////////////////////////////////////////////////////////////////////
//                              TCDB_Driver Base                             //
///////////////////////////////////////////////////////////////////////////////
TCDB_Driver::TCDB_Driver(string address_, string dirPath_)
    : dirPath(dirPath_), address(address_) {}


///////////////////////////////////////////////////////////////////////////////
//                              Git TCDB_Driver                              //
///////////////////////////////////////////////////////////////////////////////
TestCase TCDB_GitDriver::retriByIdent(string ident) {}

vector<TestCase> TCDB_GitDriver::retriByType(string type) {}

vector<TestCase> TCDB_GitDriver::retriAll() {}

bool TCDB_GitDriver::isAlive() {
    return true;
}

bool TCDB_GitDriver::active() {
    return true;
}


///////////////////////////////////////////////////////////////////////////////
//                       Local File System TCDB_Driver                       //
///////////////////////////////////////////////////////////////////////////////
namespace {
    TestCase fileToTestCase(fs::recursive_directory_iterator &) {

    }


    vector<TestCase> directoryToTestCases(fs::recursive_directory_iterator &) {

    }


    vector<TestCase> tcdbToTestCases(const string &) {

    }
}


TestCase TCDB_LocalDriver::retriByIdent(string ident) {}

vector<TestCase> TCDB_LocalDriver::retriByType(string type) {}

vector<TestCase> TCDB_LocalDriver::retriAll() {
    if (!isAlive()) {
        throw TCDB_LOCAL_DOWN("Need to Active TCDB");
    }

    try {
        if (address != dirPath) {
            fs::copy(address, dirPath, fs::copy_options::recursive);
        }
    } catch (const fs::filesystem_error &e) {
        throw TCDB_LOCAL_FAILED_TO_RETRIEVE(
            "Failed to retrieve TestCases",
            TCDB_LOCAL_FAILED_TO_RETRIEVE::filesystem_error);
    } catch (std::bad_alloc) {
        throw TCDB_LOCAL_FAILED_TO_RETRIEVE(
            "Failed to retrieve TestCases",
            TCDB_LOCAL_FAILED_TO_RETRIEVE::memAlloc_error);
    }

    vector<TestCase> tcs = tcdbToTestCases(dirPath);

    return tcs;
}

bool TCDB_LocalDriver::isAlive() {
    return isActive;
}

bool TCDB_LocalDriver::active() {
    if (!fs::exists(address)) {
        throw TCDB_NOT_EXISTS(address);
    }

    // Retrieve all TestCase in to local
    // directory.
    retriAll();
    isActive = true;

    return true;
}

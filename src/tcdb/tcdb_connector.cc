#include <memory>
#include <functional>
#include "tcdb_connector.hpp"
#include "tcdb_drivers.hpp"
#include <assert.h>


TCDB_Connector::TCDB_Connector(string address, string path, string driverName) {
    // Allocators is determine during compile time. User
    // need to make sure TCDB_Driver is supported.
    assert(tcdbDriverAllocator.contains(driverName));

    auto allocator = tcdbDriverAllocator[driverName];
    if (allocator == nullptr) {
        // Need to throw an exception
    }
    driver = allocator(address, path);
    driver->setAddress(address);
    driver->setDirPath(path);
}

bool TCDB_Connector::active() {
    return driver->active();
}


optional<TestCase> TCDB_Connector::getTC(string tcName, string group) {
    return driver->retriByIdent(tcName, group);
}


vector<TestCase> TCDB_Connector::getGroup(string grpName) {
    return driver->retriByType(grpName);
}


vector<TestCase> TCDB_Connector::getAll() {
    return driver->retriAll();
}

#include <memory>
#include <functional>
#include "tcdb_connector.h"
#include "tcdb_drivers.h"
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

void TCDB_Connector::update() {
    if (checkDriver() == false) {
        // TODO: Handle Driver incorrect status.
    }
    // Do update from TCDB
}


TestCase TCDB_Connector::getTC(string tcName) {
    update();
    return driver->retriByIdent(tcName);
}


std::vector<TestCase> TCDB_Connector::getGroup(string grpName) {
    update();
    return driver->retriByType(grpName);
}

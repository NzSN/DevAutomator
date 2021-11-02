#include <memory>
#include <functional>
#include "tcdb_connector.h"
#include "tcdb_drivers.h"

TCDB_Connector::TCDB_Connector(string address, string path, string driverName) {
    auto allocator = TCDB_ALLOCATOR[driverName];
    if (allocator == nullptr) {
        // Need to throw an exception
    }
    driver = allocator(address, path);
    driver->setAddress(address);
    driver->setDirPath(path);
}

TCDB_Connector::~TCDB_Connector() {}

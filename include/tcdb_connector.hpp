#include "general.hpp"
#include "testCase.hpp"
#include "tcdb_drivers.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <optional>


using std::unique_ptr, std::vector;

#ifndef TCDB_CONNECTOR_H
#define TCDB_CONNECTOR_H

class TCDB_Connector {
public:
    TCDB_Connector(string address,
                   string path,
                   string driverName);
    bool active();
    optional<TestCase> getTC(string tcName, string group);
    vector<TestCase> getGroup(string groupName);
    vector<TestCase> getAll();
private:
    unique_ptr<TCDB_Driver> driver;
};


#endif /* TCDB_CONNECTOR_H */

#include "general.h"
#include "testCase.h"
#include "tcdb_drivers.h"
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using std::unique_ptr;

#ifndef TCDB_CONNECTOR_H
#define TCDB_CONNECTOR_H

class TCDB_Connector {
public:
    TCDB_Connector(string address,
                   string path,
                   string driverName);
    /**
     * To Check that is driver work correctly.
     */
    bool checkDriver();
    void update();
    TestCase getTC(string tcName);
    std::vector<TestCase> getGroup(string groupName);
private:
    unique_ptr<TCDB_Driver> driver;
};


#endif /* TCDB_CONNECTOR_H */

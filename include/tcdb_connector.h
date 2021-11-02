#include "general.h"
#include "tcdb_drivers.h"
#include <memory>

using std::unique_ptr;

#ifndef TCDB_CONNECTOR_H
#define TCDB_CONNECTOR_H

class TCDB_Connector {
public:
    TCDB_Connector(string address,
                   string path,
                   string driverName);
    ~TCDB_Connector();

private:
    unique_ptr<TCDB_Driver> driver;
};


#endif /* TCDB_CONNECTOR_H */

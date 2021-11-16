#include "general.hpp"
#include <memory>
#include <string>
#include <vector>

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

class ResourceClaim {
public:
    virtual ~ResourceClaim();
    virtual std::string what();
};

class MachineResourceClaim : public ResourceClaim {

};

class ResourceManager {
public:
    bool operator()(std::shared_ptr<ResourceClaim>);
private:
    // std::vector<> resources;
};

#endif /* RESOURCEMANAGER_H */

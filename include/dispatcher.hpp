#include "general.hpp"
#include <map>
#include <memory>

#ifndef DISPATCHER_H
#define DISPATCHER_H

class Runner {};

struct RunnerLake {
    std::map<int, std::shared_ptr<Runner>> runners;
};

class Dispatcher {
public:
    Dispatcher();
};

class JobManager {
public:
    JobManager();
private:
    Dispatcher d;
    RunnerLake runners;
};


#endif /* DISPATCHER_H */

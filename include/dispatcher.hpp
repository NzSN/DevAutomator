#include "general.hpp"
#include "runner.hpp"
#include <string>
#include <map>
#include <memory>
#include "resource.hpp"
#include <boost/signals2.hpp>
#include <queue>
#include <numeric>
#include <list>
#include <thread>


#ifndef DISPATCHER_H
#define DISPATCHER_H


class Dispatcher {
public:
    void distributeTo(RunnerLake &lake);
    void doJob(std::shared_ptr<Job> job);
    void stop(std::shared_ptr<Job> job);
    void start();
private:
    boost::signals2::signal<void (OperSignal, std::shared_ptr<Job>)> sig;
    boost::signals2::signal<bool (Premise)> resSig;
    std::list<std::shared_ptr<Job>> pendings;
    std::thread t;
};


class JobManager {
public:
    JobManager();
private:
    Dispatcher d;
    RunnerLake runners;
};


#endif /* DISPATCHER_H */

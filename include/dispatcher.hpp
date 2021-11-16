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



template<Runnable T>
class Dispatcher {
public:
    void distributeTo(RunnerLake<T> &lake);
    void doJob(std::shared_ptr<Job> job);
    void stop(std::shared_ptr<Job> job);
    void start();
private:
    boost::signals2::signal<void (RunnerSignal, std::shared_ptr<Job>)> sig;
    boost::signals2::signal<bool (Premise)> resSig;
    std::list<std::shared_ptr<Job>> pendings;
    std::thread t;
};


template<Runnable T>
void Dispatcher<T>::distributeTo(RunnerLake<T> &lake) {
    auto handler = [&lake](RunnerSignal sig, std::shared_ptr<Job> j) -> void {
        lake(sig, j);
    };
    sig.connect(handler);
}


template<Runnable T>
void Dispatcher<T>::start() {

    auto dispatcherMain =
        [this]() {
            // TODO: Implement Dispatcher's main function.
        };

    t = std::move(std::thread(dispatcherMain));
}

template<Runnable T>
void Dispatcher<T>::doJob(std::shared_ptr<Job> job) {

    // Make sure Job's prerequisites is satisfied.
    Premises &p = job->premise();

    auto sourceCheck =
        [this](bool satisfied, Premise current) {
            satisfied &= *resSig(current);
            return satisfied;
        };

    bool satisfied = std::accumulate(
        p.begin(), p.end(), true, sourceCheck);

    if (satisfied) {
        sig(CREATE_NEW_RUNNER, job);
    } else {
        // Wait until all resoureces are ready.
        pendings.push_back(job);
    }
}

template<Runnable T>
void Dispatcher<T>::stop(std::shared_ptr<Job> job) {
    sig(DESTROY_RUNNER, job);
}



class JobManager {
public:
    JobManager();
private:
    Dispatcher<JobRunner> d;
    RunnerLake<JobRunner> runners;
};


#endif /* DISPATCHER_H */

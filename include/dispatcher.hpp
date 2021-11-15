#include "general.hpp"
#include "runner.hpp"
#include <string>
#include <map>
#include <memory>
#include <boost/signals2.hpp>

#ifndef DISPATCHER_H
#define DISPATCHER_H


template<typename T>
class Dispatcher {
public:
    void distributeTo(T &lake);
    void doJob(std::shared_ptr<Job> job);
    void stop(std::shared_ptr<Job> job);
private:
    boost::signals2::signal<void (RunnerSignal, std::shared_ptr<Job>)> sig;
};
template<typename T>
void Dispatcher<T>::distributeTo(T &lake) {
    auto handler = [&lake](RunnerSignal sig, std::shared_ptr<Job> j) -> void {
        lake(sig, j);
    };
    sig.connect(handler);
}

template<typename T>
void Dispatcher<T>::doJob(std::shared_ptr<Job> job) {
    sig(CREATE_NEW_RUNNER, job);
}

template<typename T>
void Dispatcher<T>::stop(std::shared_ptr<Job> job) {
    sig(DESTROY_RUNNER, job);
}



class JobManager {
public:
    JobManager();
private:
    Dispatcher<RunnerLake<JobRunner>> d;
    RunnerLake<JobRunner> runners;
};


#endif /* DISPATCHER_H */

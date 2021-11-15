#include "general.hpp"
#include "job.hpp"
#include <string>
#include <memory>
#include <map>
#include <exception>

#ifndef RUNNER_H
#define RUNNER_H

typedef enum RunnerSignal {
    IDLE = 0,
    CREATE_NEW_RUNNER,
    DESTROY_RUNNER,
    DESTROY_ALL_RUNNERS,
} RunnerSignal;

class Runner {
public:
    virtual ~Runner() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    void setIdent(std::string ident_) { ident = ident_; }
    std::string getIdent() const { return ident; };
private:
    std::string ident;
};

class JobRunner : public Runner {
public:
    JobRunner(Job &j);
    ~JobRunner() {}
    void start();
    void stop();
};

template<typename _Runner_T>
class RunnerLake {
public:
    void create(Job &j);
    void start(Job &j);
    void stop(Job &j);
    void stopAll();
    bool exists(Job &j);
    virtual void operator()(RunnerSignal sig, std::shared_ptr<Job>);
private:
    std::shared_ptr<_Runner_T> factory(Job &j) const;
    std::map<std::string, std::shared_ptr<_Runner_T>> runners;
};


template<typename _Runner_T>
std::shared_ptr<_Runner_T> RunnerLake<_Runner_T>::factory(Job &j) const {
    return std::make_shared<_Runner_T>(j);
}


template<typename _Runner_T>
void RunnerLake<_Runner_T>::create(Job &j) {
    std::string ident = j.getIdent();

    if (runners.contains(ident)) {
        throw std::runtime_error("Runner("+ ident +")already exists");
    }

    runners[ident] = factory(j);
}

template<typename _Runner_T>
void RunnerLake<_Runner_T>::start(Job &j) {
    std::string ident = j.getIdent();

    if (!runners.contains(ident)) {
        throw std::runtime_error("Runner (" + ident + ") not found");
    }

    runners[ident]->start();
}


template<typename _Runner_T>
void RunnerLake<_Runner_T>::stop(Job &j) {
    std::string ident = j.getIdent();

    if (!runners.contains(ident)) {
        throw std::runtime_error("Runner (" + ident + ") not found");
    }

    runners[ident]->stop();
}


template<typename _Runner_T>
void RunnerLake<_Runner_T>::stopAll() {
    for (auto &i : runners)
        i.second->stop();
}


template<typename _Runner_T>
bool RunnerLake<_Runner_T>::exists(Job &j) {
    std::string ident = j.getIdent();
    return runners.contains(ident);
}

template<typename _Runner_T>
void RunnerLake<_Runner_T>::operator()(RunnerSignal sig, std::shared_ptr<Job> job) {

    try {
        switch (static_cast<int>(sig)) {
        case CREATE_NEW_RUNNER:
            create(*job);
            start(*job);
            break;
        case DESTROY_RUNNER:
            stop(*job);
            break;
        case DESTROY_ALL_RUNNERS:
            stopAll();
            break;
      }
    } catch (std::exception &e) {
        std::string msg = e.what();
        throw std::runtime_error(
            "RunnerLake failed with op: " + std::to_string(sig) + "\n"
            "Message:\n" +  msg);
    }
}


#endif /* RUNNER_H */

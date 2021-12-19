#include "dispatcher.hpp"


void Dispatcher::distributeTo(RunnerLake &lake) {
    auto handler = [&lake](OperSignal sig, std::shared_ptr<Job> j) -> void {
        lake(sig, j);
    };
    sig.connect(handler);
}



void Dispatcher::start() {

    auto dispatcherMain =
        [this]() {
            // TODO: Implement Dispatcher's main function.
        };

    t = std::move(std::thread(dispatcherMain));
}


void Dispatcher::doJob(std::shared_ptr<Job> job) {

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


void Dispatcher::stop(std::shared_ptr<Job> job) {
    sig(DESTROY_RUNNER, job);
}


JobManager::JobManager() {
    d.distributeTo(runners);
}

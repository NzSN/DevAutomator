#include "general.hpp"
#include "runner.hpp"
#include <exception>
#include "job.hpp"

///////////////////////////////////////////////////////////////////////////////
//                           Runner Implementations                          //
///////////////////////////////////////////////////////////////////////////////
Runner::~Runner() {}


///////////////////////////////////////////////////////////////////////////////
//                       RunnerCommand Implementations                       //
///////////////////////////////////////////////////////////////////////////////
RunnerCommand::~RunnerCommand() {}

void RCmdCreate::exec(RunnerLake &lake) {}

void RCmdDestroy::exec(RunnerLake &lake) {}

void RCmdDestroyAll::exec(RunnerLake &lake) {}

std::unique_ptr<RunnerCommand> RunnerCommandFactory::makeCommand(OperSignal sig, Job &job) {
    std::unique_ptr<RunnerCommand> cmd;

    switch (static_cast<int>(sig)) {
    case CREATE_NEW_RUNNER:
        break;
    case DESTROY_RUNNER:
        break;
    case DESTROY_ALL_RUNNERS:
        break;
    }

    return cmd;
}


///////////////////////////////////////////////////////////////////////////////
//                         RunnerLake Implementations                        //
///////////////////////////////////////////////////////////////////////////////
void RunnerLake::create(Job &j) {
    std::string ident = j.getIdent();

    if (runners.contains(ident)) {
        throw std::runtime_error("Runner("+ ident +")already exists");
    }

    runners[ident] = factory->makeRunner(j);
}

void RunnerLake::start(Job &j) {
    std::string ident = j.getIdent();

    if (!runners.contains(ident)) {
        throw std::runtime_error("Runner (" + ident + ") not found");
    }

    runners[ident]->start();
}


void RunnerLake::stop(Job &j) {
    std::string ident = j.getIdent();

    if (!runners.contains(ident)) {
        throw std::runtime_error("Runner (" + ident + ") not found");
    }

    runners[ident]->stop();
}


void RunnerLake::stopAll() {
    for (auto &i : runners)
        i.second->stop();
}


bool RunnerLake::exists(Job &j) {
    std::string ident = j.getIdent();
    return runners.contains(ident);
}

void RunnerLake::setFactory(std::shared_ptr<RunnerFactory> factory_) {
    factory = factory_;
}

void RunnerLake::operator()(OperSignal sig, std::shared_ptr<Job> job) {
    std::unique_ptr<RunnerCommand> cmd =
        RunnerCommandFactory::makeCommand(sig, *job);
    cmd->exec(*this);
}

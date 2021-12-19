#include "general.hpp"
#include "job.hpp"
#include <string>
#include <memory>
#include <map>
#include <exception>

#ifndef RUNNER_H
#define RUNNER_H

typedef enum OperSignal {
    UNUSED = 0,
    CREATE_NEW_RUNNER,
    DESTROY_RUNNER,
    DESTROY_ALL_RUNNERS,
} OperSignal;



///////////////////////////////////////////////////////////////////////////////
//                             Runner Definitions                            //
///////////////////////////////////////////////////////////////////////////////
class Runner {
public:
    virtual ~Runner();
    virtual void start() = 0;
    virtual void stop() = 0;
    void setIdent(std::string ident_) { ident = ident_; }
    std::string getIdent() const { return ident; };

  private:
    std::string ident;
};

class RunnerFactory {
public:
    virtual std::shared_ptr<Runner> makeRunner(Job&) = 0;
};

///////////////////////////////////////////////////////////////////////////////
//                           RunnerLake Definitions                          //
///////////////////////////////////////////////////////////////////////////////
class RunnerLake {
public:
    void create(Job &j);
    void start(Job &j);
    void stop(Job &j);
    void stopAll();
    bool exists(Job &j);
    void setFactory(std::shared_ptr<RunnerFactory>);
    virtual void operator()(OperSignal sig, std::shared_ptr<Job>);
private:
    std::shared_ptr<RunnerFactory> factory;
    std::map<std::string, std::shared_ptr<Runner>> runners;
};


///////////////////////////////////////////////////////////////////////////////
//                         RunnerCommand Definitions                         //
///////////////////////////////////////////////////////////////////////////////
class RunnerCommand {
public:
    virtual ~RunnerCommand();
    virtual void exec(RunnerLake&) = 0;
private:
    std::shared_ptr<Job> job;
};

class RCmdCreate : public RunnerCommand {
public:
    void exec(RunnerLake&);
};

class RCmdDestroy : public RunnerCommand {
public:
    void exec(RunnerLake&);
};

class RCmdDestroyAll : public RunnerCommand {
public:
    void exec(RunnerLake&);
};

class RunnerCommandFactory {
public:
    static std::unique_ptr<RunnerCommand> makeCommand(OperSignal, Job &job);
};


#endif /* RUNNER_H */

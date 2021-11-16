#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "dispatcher.hpp"
#include "runner.hpp"


class TrivialLake : public RunnerLake<JobRunner> {
public:
    void create(Job &j) {
        sig = CREATE_NEW_RUNNER;
    }
    void stop(Job &j) {
        sig = DESTROY_RUNNER;
    }
    void stopAll() {
        sig = DESTROY_ALL_RUNNERS;
    }

    TrivialLake() = default;

    void operator()(RunnerSignal sig, std::shared_ptr<Job> j) {
        switch (static_cast<int>(sig)) {
        case CREATE_NEW_RUNNER:
            create(*j);
            break;
        case DESTROY_RUNNER:
            stop(*j);
            break;
        case DESTROY_ALL_RUNNERS:
            stopAll();
        }
    }
    RunnerSignal sig = IDLE;
};

class Dispatcher_Fixture : public ::testing::Test {
protected:
    Dispatcher<JobRunner> d;
};


TEST_F(Dispatcher_Fixture, DistributeToLake) {}

TEST_F(Dispatcher_Fixture, dispatch) {
    TrivialLake lake;
    d.distributeTo(lake);

    std::shared_ptr<Job> j = std::make_shared<Job>();
    d.doJob(j);

    // Verify
    EXPECT_EQ(lake.sig, CREATE_NEW_RUNNER);
}

TEST_F(Dispatcher_Fixture, stop) {
    /**
     * Assume that Runner is able to
     * be stoped immediately.
     */

    TrivialLake lake;
    d.distributeTo(lake);

    std::shared_ptr<Job> j = std::make_shared<Job>();
    d.doJob(j);

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100));

    d.stop(j);

    // Verify
    EXPECT_EQ(lake.sig, DESTROY_RUNNER);
}

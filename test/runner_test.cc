#include "gtest/gtest.h"
#include "runner.hpp"
#include "job.hpp"


typedef enum {
    R_IDLE = 0,
    R_STOP,
    R_START,
} State;

class TrivialRunner : public Runner {
public:
    TrivialRunner(Job &j) {}
    ~TrivialRunner() {}
    void start() { state = R_START; }
    void stop() { state = R_STOP; }

private:
    int state = R_STOP;
};


class RunnerLake_Fixture : public ::testing::Test {
protected:
    RunnerLake lake;
};


TEST_F(RunnerLake_Fixture, create) {
    Job j {"ID"};
    lake.create(j);

    EXPECT_TRUE(lake.exists(j));
}

TEST_F(RunnerLake_Fixture, create_redundant) {
    /**
     * l should failed to be created
     */
    Job j{"ID"}, l{"ID"};
    lake.create(j);

    try {
      lake.create(l);
      FAIL();
    } catch (std::runtime_error &e) {}
}

TEST_F(RunnerLake_Fixture, start) {}

TEST_F(RunnerLake_Fixture, stop) {}

TEST_F(RunnerLake_Fixture, stopAll) {}

TEST_F(RunnerLake_Fixture, Callable) {}

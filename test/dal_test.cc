#include <gtest/gtest.h>
#include "DAL/instruction.hpp"
#include "DAL/environment.hpp"


class InstSet_Fixture : public ::testing::Test {
protected:
    InstructionSet insts;
    DAL_Environment env;
};


TEST_F(InstSet_Fixture, Iterator) {

    for (auto &iter: insts) {
        // Check
    }
}


TEST_F(InstSet_Fixture, Evaluation) {
    insts.eval(env);

    // Do checking on Environment
}

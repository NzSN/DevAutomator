#include <gtest/gtest.h>
#include "DAL/instruction.hpp"
#include "DAL/environment.hpp"


class InstSet_Fixture : public ::testing::Test {
protected:
    InstructionSet insts;
    DAL_Environment env;
};

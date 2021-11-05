#include <gtest/gtest.h>
#include <filesystem>
#include "tcdb_drivers.hpp"
#include <iostream>


class TCDB_LocalDrivers: public ::testing::Test {
protected:
    TCDB_LocalDriver driver = {"/tmp/TCDB_REMOTE", "/tmp/TCDB_LOCAL"};
};



TEST_F(TCDB_LocalDrivers, DriverActive) {
    driver.active();
    bool tcdbExists = std::filesystem::exists(driver.getDirPath());
    EXPECT_EQ(tcdbExists, true);
    EXPECT_EQ(driver.isAlive(), true);
}

TEST_F(TCDB_LocalDrivers, DriverStatus) {
    EXPECT_EQ(driver.isAlive(), true);
}

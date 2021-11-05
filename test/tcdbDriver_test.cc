#include <gtest/gtest.h>
#include <filesystem>
#include "tcdb_drivers.hpp"
#include <iostream>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

using std::string;


class TCDB_LocalDrivers: public ::testing::Test {
    /**
     * This fixture create a TCDB with three groups:
     * + Group1
     * + Group2
     * + Group3
     *
     * Each of groups contains three TestCases
     *
     * Sturcture looks like:
     *
     * DB
     *  |
     *  |--- Group1 ---- { t1, t2, t3 }
     *  |
     *  |--- Group2 ---- { t1, t2, t3 }
     *  |
     *  |--- Group3 ---- { t1, t2, t3 }
     *
     */
protected:
    void SetUp() override {
        // Create TCDB Directory
        fs::create_directory(dbAddress);

        // Create TCDB Groups
        fs::create_directory(dbAddress + group1);
        fs::create_directory(dbAddress + group2);
        fs::create_directory(dbAddress + group3);

        // Create TestCases
        for (auto &i : {group1, group2, group3}) {
          std::ofstream tc1(dbAddress + i + "/" + testCase1);
          tc1 << i << std::endl;

          std::ofstream tc2(dbAddress + i + "/" + testCase2);
          tc2 << i << std::endl;

          std::ofstream tc3(dbAddress + i + "/" + testCase3);
          tc3 << i << std::endl;
        }
    }

    void TearDown() override {
        fs::remove_all(dbAddress);
    }

    string group1 = "Group1";
    string group2 = "Group2";
    string group3 = "Group3";

    string testCase1 = "tc1";
    string testCase2 = "tc2";
    string testCase3 = "tc3";

    string dbAddress = "/tmp/TCDB_REMOTE/";
    string dbLocalDir = "/tmp/TCDB_LOCAL/";
    TCDB_LocalDriver driver = {dbAddress, dbLocalDir};
};


TEST_F(TCDB_LocalDrivers, DriverActiveCheck) {

    // Active the TCDB
    driver.active();

    // Verify that TCDB is actived
    bool tcdbExists = fs::exists(driver.getDirPath());
    EXPECT_EQ(tcdbExists, true);
    EXPECT_EQ(driver.isAlive(), true);
}


TEST_F(TCDB_LocalDrivers, DefaultStatusCheck) {
    EXPECT_EQ(driver.isAlive(), false);
}


TEST_F(TCDB_LocalDrivers, TCDB_FileSystemStructureCheck) {
    // Active the TCDB
    driver.active();

    // Check directory on filesystem
    EXPECT_TRUE(fs::exists(dbLocalDir));
    EXPECT_TRUE(fs::exists(dbLocalDir + group1));
    EXPECT_TRUE(fs::exists(dbLocalDir + group2));
    EXPECT_TRUE(fs::exists(dbLocalDir + group3));

    for (auto &i : { group1, group2, group3 }) {
        EXPECT_TRUE(fs::exists(dbLocalDir + i + '/' + testCase1));
        EXPECT_TRUE(fs::exists(dbLocalDir + i + '/' + testCase2));
        EXPECT_TRUE(fs::exists(dbLocalDir + i + '/' + testCase3));
    }
}


TEST_F(TCDB_LocalDrivers, TCDB_DataCheck) {
    // Active the TCDB
    driver.active();

    std::optional<TestCase> tc;
    std::ifstream tcIn;
    string content;

    // First, retrieve single TestCase.
    for (auto &i : {group1, group2, group3}) {
        for (auto &j: {testCase1, testCase2, testCase3}) {
            tc = driver.retriByIdent(j, i);
            EXPECT_TRUE(tc.has_value());
            // To check it's content to
            // verify that it's in corrected group.
            tcIn.open(tc->path());
            tcIn >> content;
            tcIn.close();

            EXPECT_EQ(i, content);
        }
    }

    // Then to check retriByType
    for (auto &i : {group1, group2, group3}) {
        vector<TestCase> &&groupOfTcs = driver.retriByType(i);
        EXPECT_EQ(groupOfTcs.size(), 3);

        // Check Group
        for (auto &tc : groupOfTcs) {
            tcIn.open(tc.path());
            tcIn >> content;
            tcIn.close();
            EXPECT_EQ(i, content);
        }
    }

    // Check retriAll
    vector<TestCase> &&tcs = driver.retriAll();
    EXPECT_EQ(tcs.size(), 9);
}

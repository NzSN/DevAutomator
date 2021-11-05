#include <gtest/gtest.h>
#include <filesystem>
#include "tcdb_connector.hpp"
#include <fstream>

namespace fs = std::filesystem;


class TCDB_ConnectorWithLocal_Fixture: public ::testing::Test {
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

    string dbAddress = "/tmp/TCDB_CONNEC_REMOTE/";
    string dbLocalDir = "/tmp/TCDB_CONNEC_LOCAL/";
    TCDB_Connector connector = {dbAddress, dbLocalDir, "local"};
};

TEST_F(TCDB_ConnectorWithLocal_Fixture, Active) {
    connector.active();

    std::string content;
    std::ifstream tcIn;

    for (auto &grp : {group1, group2, group3}) {
        for (auto &tc : {testCase1, testCase2, testCase3}) {
            string path = dbAddress + grp + "/" + tc;
            EXPECT_TRUE(fs::exists(path));

            tcIn.open(path);
            tcIn >> content;
            tcIn.close();
            EXPECT_EQ(content, grp);
        }
    }
}

TEST_F(TCDB_ConnectorWithLocal_Fixture, Init) {}

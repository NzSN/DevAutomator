#include <gtest/gtest.h>
#include "tcdb.hpp"
#include "testCase.hpp"


class EMPTY_TCDB_Fixture: public ::testing::Test {
protected:
    TCDB driver = {};
};


class TCDB_10_100_Fixture: public ::testing::Test {
protected:
    void SetUp() override {
        // Add datas into TCDB
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 100; j++) {
                driver.addTC(TestCase(std::to_string(j), std::to_string(i)));
            }
        }
    }

    TCDB driver = {};
};


TEST_F(EMPTY_TCDB_Fixture, AddTestCasesCheck) {
    constexpr int count = 10000;

    for (int i = 0; i < count; i++) {
        driver.addTC(TestCase(std::to_string(i), std::to_string(i)));
    }

    EXPECT_EQ(driver.size(), count);
}


TEST_F(TCDB_10_100_Fixture, GetTestCaseCheck) {
    string grp, name;
    optional<TestCase> tc;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 100; j++) {
            grp = std::to_string(i);
            name = std::to_string(j);

            tc = driver.getTC(name, grp);
            EXPECT_EQ(tc.has_value(), true);
            EXPECT_EQ(tc.value().name(), name);
            EXPECT_EQ(tc.value().group(), grp);
        }
    }
}

TEST_F(TCDB_10_100_Fixture, GetGroupOfTestCasesCheck) {
    for (int i = 0; i < 10; i++) {
        vector<TestCase> tc = driver.getGroup(std::to_string(i));
        EXPECT_EQ(tc.size(), 100);

        for (int j = 0; j < 100; j++)
            EXPECT_EQ(std::to_string(j), tc[j].name());
    }
}

TEST_F(TCDB_10_100_Fixture, GetAllTestCasesCheck) {
    vector<TestCase> tcs = driver.getAll();
    EXPECT_EQ(tcs.size(), 1000);
}

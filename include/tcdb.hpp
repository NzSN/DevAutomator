#include "general.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <optional>
#include "testCase.h"
#include <optional>

using std::map, std::string, std::vector, std::optional;

#ifndef TCDB_H
#define TCDB_H

class TCDB {
public:
    TCDB() = default;
    void addTC(TestCase tc);
    optional<TestCase> getTC(string group, string tcName);
    optional<vector<TestCase>> getGroup(string grpName);
    optional<vector<TestCase>> getAll();
    size_t size() const {
        size_t total = 0;
        for (auto& i: db) {
            total += i.second.size();
        }

        return total;
    }
private:
    map<string, vector<TestCase>> db;
};


#endif /* TCDB_H */

#include "general.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <optional>
#include "testCase.hpp"
#include <optional>

using std::map, std::string, std::vector, std::optional;

#ifndef TCDB_H
#define TCDB_H

class TCDB {
public:
    TCDB() = default;
    ~TCDB() {}
    void addTC(TestCase tc);
    optional<TestCase> getTC(string tcName, string group);
    vector<TestCase> getGroup(string grpName);
    vector<TestCase> getAll();
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

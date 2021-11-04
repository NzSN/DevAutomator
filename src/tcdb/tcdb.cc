#include "general.h"
#include "tcdb.hpp"
#include <algorithm>
#include <iterator>


void TCDB::addTC(TestCase tc) {
    string grp = tc.group();

    // TODO: Exception may throw during push_back
    db[grp].push_back(tc);
}

optional<TestCase> TCDB::getTC(string tcName, string group) {
    if (!db.contains(group)) {
        return std::nullopt;
    }

    vector<TestCase> groupOfTcs = db[group];
    auto tc = std::find_if(begin(groupOfTcs), end(groupOfTcs), [tcName](TestCase tc_) {
        return tc_.name() == tcName;
    });

    if (tc == end(groupOfTcs)) {
        return std::nullopt;
    }

    return *tc;
}


optional<vector<TestCase>>
TCDB::getGroup(string grpName) {
    if (!db.contains(grpName)) {
        return std::nullopt;
    }

    return db[grpName];
}


optional<vector<TestCase>>
TCDB::getAll() {
    vector<TestCase> tcs;

    if (db.size() == 0) {
        return std::nullopt;
    }

    for (auto& i: db) {
        tcs.insert(end(tcs), i.second.begin(), i.second.end());
    }

    return tcs;
}

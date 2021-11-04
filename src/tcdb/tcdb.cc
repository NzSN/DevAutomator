#include "general.h"
#include "tcdb.hpp"
#include <algorithm>
#include <iterator>


void TCDB::addTC(TestCase tc) {
    // TODO: Group name format checking
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
        return {};
    }

    return *tc;
}


vector<TestCase>
TCDB::getGroup(string grpName) {
    if (!db.contains(grpName)) {
        return {};
    }

    return db[grpName];
}


vector<TestCase>
TCDB::getAll() {
    vector<TestCase> tcs;

    if (db.size() == 0) {
        return {};
    }

    for (auto& i: db) {
        tcs.insert(end(tcs), i.second.begin(), i.second.end());
    }

    return tcs;
}

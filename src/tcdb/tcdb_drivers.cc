#include "tcdb_drivers.hpp"
#include "tcdb.hpp"
#include <filesystem>
#include <exception>
#include <assert.h>
#include <stack>
#include <utility>
#include <optional>

namespace fs = std::filesystem;
using std::vector, std::stack, std::pair;
using std::optional;


TCDB_ALLOCATOR tcdbDriverAllocator = {
    {"git", [](string addr, string path) -> unique_ptr<TCDB_Driver> {
            return std::make_unique<TCDB_GitDriver>(addr, path); }},
    {"local", [](string addr, string path) -> unique_ptr<TCDB_Driver> {
            return std::make_unique<TCDB_LocalDriver>(addr, path); }},
};

///////////////////////////////////////////////////////////////////////////////
//                              TCDB_Driver Base                             //
///////////////////////////////////////////////////////////////////////////////
TCDB_Driver::TCDB_Driver(string address_, string dirPath_)
    : dirPath(dirPath_), address(address_) {}


///////////////////////////////////////////////////////////////////////////////
//                              Git TCDB_Driver                              //
///////////////////////////////////////////////////////////////////////////////
optional<TestCase> TCDB_GitDriver::retriByIdent(string ident, string type) {}

vector<TestCase> TCDB_GitDriver::retriByType(string type) {}

vector<TestCase> TCDB_GitDriver::retriAll() {}

bool TCDB_GitDriver::isAlive() {
    return true;
}

bool TCDB_GitDriver::active() {
    return true;
}


///////////////////////////////////////////////////////////////////////////////
//                       Local File System TCDB_Driver                       //
///////////////////////////////////////////////////////////////////////////////
namespace {
    /**
     * Assume that the file correspond with path is a directory.
     */
    void buildTCDB(TCDB &db, const string &path) {
        auto ii = fs::recursive_directory_iterator(path);
        stack<pair<string, int>> groupStatus;

        // Setup Initial state
        // All TestCases without Group will
        // reside in 'Default' Group.
        groupStatus.push({"Default", 0});

        for (auto i = fs::recursive_directory_iterator(path);
             i != fs::recursive_directory_iterator(); ++i) {

          // Depth Checking
          int currentDepth = groupStatus.top().second;
          if (currentDepth > 1) {
            // TODO: This disobey the rule of TCDB.
            throw TCDB_ERROR_FORM("TCDB_LocalDriver: Directory's depth \
                                   level must not bigger than 1.");
          }
          if (i.depth() < currentDepth) {
            groupStatus.pop();
          }

          if (i->is_regular_file()) {
            // Create Testcase
            TestCase tc = {// TestCase Name
                           i->path().filename(),
                           // TestCase Group
                           groupStatus.top().first,
                           // TestCase File Path
                           i->path()};

            db.addTC(tc);
          } else if (i->is_directory()) {
            // Goto Group
            groupStatus.push({
                    // Directory's name is the name of
                    // group.
                    i->path().filename(),
                    // Go into subdir
                    i.depth() + 1 });
          }
        }
    }

    void tcdbClone(TCDB &db, const string &address, const string &dirPath) {

        try {
            if (address != dirPath) {
                fs::copy(address, dirPath, fs::copy_options::recursive);
            }
        } catch (const fs::filesystem_error &e) {
            throw TCDB_LOCAL_FAILED_TO_RETRIEVE(
                "Failed to retrieve TestCases",
                TCDB_LOCAL_FAILED_TO_RETRIEVE::filesystem_error);
        } catch (std::bad_alloc &e) {
            throw TCDB_LOCAL_FAILED_TO_RETRIEVE(
                "Failed to retrieve TestCases",
                TCDB_LOCAL_FAILED_TO_RETRIEVE::memAlloc_error);
        }

        // Rebuild TCDB
        buildTCDB(db, dirPath);
    }
}

optional<TestCase> TCDB_LocalDriver::retriByIdent(string ident, string type) {
    return db.getTC(ident, type);
}

vector<TestCase> TCDB_LocalDriver::retriByType(string type) {
    return db.getGroup(type);
}

vector<TestCase> TCDB_LocalDriver::retriAll() {
    if (!isAlive()) {
        throw TCDB_LOCAL_DOWN("Need to Active TCDB");
    }

    return db.getAll();
}

bool TCDB_LocalDriver::isAlive() {
    return isActive;
}

bool TCDB_LocalDriver::active() {
    if (!fs::exists(address) || !fs::is_directory(address)) {
        throw TCDB_NOT_EXISTS(address);
    }

    try {
        // Clone TCDB from address to dirPath
        // and buildup TCDB object.
        tcdbClone(db, address, dirPath);
    } catch (const TCDB_LOCAL_FAILED_TO_RETRIEVE &e) {
        switch (e.reason()) {
        case TCDB_LOCAL_FAILED_TO_RETRIEVE::filesystem_error: {
            throw TCDB_LOCAL_FAILED_TO_ACTIVE("Filesystem error");
            break;
        }
        case TCDB_LOCAL_FAILED_TO_RETRIEVE::memAlloc_error: {
            throw TCDB_LOCAL_FAILED_TO_ACTIVE("Memory allocation error");
            break;
        }
        default:
            break;
        }
    }

    isActive = true;

    return true;
}

TCDB_LocalDriver::~TCDB_LocalDriver() {
    fs::remove_all(dirPath);
}

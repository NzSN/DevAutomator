/**
 * 2021/11/5:
 * TODO: Provide Synchronization to TCDB_LocalDriver.
 *     TCDB in Filesystem need to provide a file that contain
 *     meta datas of it and Driver able to sync TCDB with helps
 *     from the meta file.
 */



#include "testCase.hpp"
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <initializer_list>
#include <vector>
#include <utility>
#include <exception>
#include "tcdb.hpp"

using std::string;
using std::unique_ptr;


#ifndef TCDB_DRIVERS_H
#define TCDB_DRIVERS_H


///////////////////////////////////////////////////////////////////////////////
//                                TCDB_Driver                                //
///////////////////////////////////////////////////////////////////////////////
class TCDB_NOT_EXISTS: public std::runtime_error {
public:
    explicit TCDB_NOT_EXISTS(const string address_):
        std::runtime_error(address_) {}
};


class TCDB_ERROR_FORM: public std::runtime_error {
public:
  explicit TCDB_ERROR_FORM(const string address_)
      : std::runtime_error(address_) {}
};


class TCDB_Driver {
public:
    TCDB_Driver() = default;
    TCDB_Driver(string address, string dirPath);
    // Retrieve TestCase by it's identifier
    virtual optional<TestCase> retriByIdent(string ident, string type) = 0;
    // Retrieve all TestCases of the given type
    virtual std::vector<TestCase> retriByType(string type) = 0;
    virtual std::vector<TestCase> retriAll() = 0;
    virtual bool isAlive() = 0;
    virtual bool active() = 0;
    void   setDirPath(string path) { dirPath = path; }
    string getDirPath() const      { return dirPath; }
    void   setAddress(string addr) { address = addr; }
protected:
    // Path of directory to store retrieved TestCases
    string dirPath;
    // Address Information to connect to TCDB.
    string address;
    TCDB db;
};


///////////////////////////////////////////////////////////////////////////////
//                               TCDB_GitDriver                              //
///////////////////////////////////////////////////////////////////////////////
class TCDB_GitDriver: public TCDB_Driver {
    /**
     * A TCDB Driver that connect to a TCDB which is
     * a Git Repository.
     */
public:
    using TCDB_Driver::TCDB_Driver;

    optional<TestCase> retriByIdent(string ident, string type) override;
    std::vector<TestCase> retriByType(string type) override;
    std::vector<TestCase> retriAll() override;
    bool isAlive() override;
    bool active() override;

    void setGitUrl(string url) { gitUrl = url; }
    string getGitUrl() const { return gitUrl; }
    void setPublicKey(string key) { publicKey = key; }
    string getPublicKey() const { return publicKey; }

private:
    string gitUrl;
    string publicKey;
};


///////////////////////////////////////////////////////////////////////////////
//                              TCDB_LocalDriver                             //
///////////////////////////////////////////////////////////////////////////////
class TCDB_LOCAL_DOWN: public std::runtime_error {
public:
  explicit TCDB_LOCAL_DOWN(const string s) : runtime_error(s) {}
};


class TCDB_LOCAL_FAILED_TO_RETRIEVE: public std::runtime_error {
public:
    constexpr static int filesystem_error = 0;
    constexpr static int memAlloc_error = 1;
    explicit TCDB_LOCAL_FAILED_TO_RETRIEVE(const string s, int reason): runtime_error(s) {}
    int reason() const {
        return error_type;
    }
private:
    int error_type;;
};


class TCDB_LOCAL_FAILED_TO_ACTIVE: public std::runtime_error {
public:
    explicit TCDB_LOCAL_FAILED_TO_ACTIVE(const string s) : runtime_error(s) {}
};


class TCDB_LocalDriver: public TCDB_Driver {
    /**
     * A TCDB Driver that use a local file system directory
     * as TCDB.
     */
public:
    using TCDB_Driver::TCDB_Driver;
    ~TCDB_LocalDriver();

    optional<TestCase> retriByIdent(string ident, string type) override;
    std::vector<TestCase> retriByType(string type) override;
    std::vector<TestCase> retriAll() override;
    bool isAlive() override;
    bool active() override;
private:
    bool isActive = false;
};


class TCDB_ALLOCATOR {
public:
    using DriverFactory = std::function<unique_ptr<TCDB_Driver>(string, string)>;
    TCDB_ALLOCATOR(std::initializer_list<std::pair<string, DriverFactory>> list) {
        for (auto iter: list) {
            driverFactories[iter.first] = iter.second;
        }
    }
    bool contains(string driverName) const {
        return driverFactories.count(driverName) > 0;
    }
    DriverFactory& operator [](string driverName) {
        return driverFactories[driverName];
    }
private:
    std::map<string, DriverFactory> driverFactories;
};

extern TCDB_ALLOCATOR tcdbDriverAllocator;

#endif /* TCDB_DRIVERS_H */

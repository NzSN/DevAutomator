#include <string>
#include <map>
#include <memory>
#include <functional>

using std::string;
using std::unique_ptr;


#ifndef TCDB_DRIVERS_H
#define TCDB_DRIVERS_H


class TCDB_Driver {
public:
    TCDB_Driver(string address, string dirPath);
    // Retrieve TestCase by it's identifier
    virtual void retriByIdent(string ident) = 0;
    // Retrieve all TestCases of the given type
    virtual void retriByType(string type) = 0;
    virtual void retriAll() = 0;
    virtual bool isAlive() = 0;
    virtual bool active() = 0;
    void   setDirPath(string path) { dirPath = path; }
    string getDirPath() const      { return dirPath; }
    void   setAddress(string addr) { address = addr; }
private:
    // Path of directory to store retrieved TestCases
    string dirPath;
    // Address Information to connect to TCDB.
    string address;
};


class TCDB_GitDriver: public TCDB_Driver {
    /**
     * A TCDB Driver that connect to a TCDB which is
     * a Git Repository.
     */
public:
    using TCDB_Driver::TCDB_Driver;

    void retriByIdent(string ident) override;
    void retriByType(string type) override;
    void retriAll() override;
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


class TCDB_LocalDriver: public TCDB_Driver {
    /**
     * A TCDB Driver that use a local file system directory
     * as TCDB.
     */
public:
    using TCDB_Driver::TCDB_Driver;

    void retriByIdent(string ident) override;
    void retriByType(string type) override;
    void retriAll() override;
    bool isAlive() override;
    bool active() override;

    void setSubDir(string path) { subDir = path; }
    string getSubDir() { return subDir; }
private:
    string subDir;
};

class TCDB_ALLOCATOR {
public:
    using DriverFactory = std::function<unique_ptr<TCDB_Driver>(string, string)>;
    DriverFactory& operator [](string driverName) {
        return driverFactories[driverName];
    }
private:
    std::map<string, DriverFactory> driverFactories;
};

extern TCDB_ALLOCATOR globalTcdbDriverAllocator;

#endif /* TCDB_DRIVERS_H */

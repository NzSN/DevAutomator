#include "general.hpp"
#include <string>


#ifndef JOB_H
#define JOB_H


class Job {
public:
    Job() = default;
    Job(std::string id): ident(id) {}
    std::string getIdent() {
        return ident;
    }
private:
    std::string ident;
};

#endif /* JOB_H */

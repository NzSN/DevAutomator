#include "general.hpp"
#include "resource.hpp"
#include <string>
#include <vector>
#include <memory>


#ifndef JOB_H
#define JOB_H

using Premise = std::shared_ptr<ResourceClaim>;
using Premises = std::vector<Premise>;

class Job {
public:
    Job() = default;
    Job(std::string id): ident(id) {}
    std::string getIdent() {
        return ident;
    }

    Premises& premise() {
        return reqSources;
    }
private:
    std::string ident;
    Premises reqSources;
};

#endif /* JOB_H */

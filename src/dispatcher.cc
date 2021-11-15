#include "dispatcher.hpp"


JobManager::JobManager() {
    d.distributeTo(runners);
}

#pragma once

#include "Worker.h"

#include <mutex>
#include <set>

class BaseServer: public IBaseService
{
    std::set<Worker*> connections_;
    std::mutex connsetMutex;
    IGlobalService *server_;

public:
    BaseServer(IGlobalService *server);
    ~BaseServer();
    void stopConnections();
    void registerConnection(Worker *w) override;
    void deregisterConnection(Worker *w) override;
};

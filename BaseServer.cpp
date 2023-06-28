#include <iostream>

#include "BaseServer.h"

BaseServer::BaseServer(IGlobalService *server)
:server_(server)
{
}

BaseServer::~BaseServer()
{
}

void BaseServer::stopConnections()
{
    for (auto conn:connections_)
    {
        conn->stop();
    }
    connections_.clear();
}

void BaseServer::registerConnection(Worker *w)
{
    std::lock_guard<std::mutex> guard(connsetMutex);
    connections_.insert(w);
}

void BaseServer::deregisterConnection(Worker *w)
{
    std::lock_guard<std::mutex> guard(connsetMutex);
    connections_.erase(w);
    server_->deleteWorker(w);
}

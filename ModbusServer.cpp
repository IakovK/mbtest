#include <iostream>
#include "ThreadDeleter.h"
#include "ModbusServer.h"
#include "ModbusConnection.h"

#define NB_CONNECTION 5

ModbusServer::ModbusServer(int port, IGlobalService *server)
:port_(port)
,server_(server)
{
}

ModbusServer::~ModbusServer()
{
    if (mb_mapping_ != nullptr)
        modbus_mapping_free(mb_mapping_);
    if (ctx_ != nullptr)
        modbus_free(ctx_);
}

void ModbusServer::run__()
{
    std::cout << "ModbusServer::run__" << "\n";
    ctx_ = modbus_new_tcp(nullptr, 1502);
    if (ctx_ == nullptr)
    {
        std::cout << "failed to create modbus context. exiting" << "\n";
        return;
    }
    mb_mapping_ = modbus_mapping_new(10, 10, 10, 10);
    if (mb_mapping_ == nullptr)
    {
        std::cout << "failed to create modbus mapping. exiting" << "\n";
        return;
    }
    server_socket_ = modbus_tcp_listen(ctx_, NB_CONNECTION);
    if (server_socket_ == -1)
    {
        std::cout << "Unable to listen TCP connection. exiting" << "\n";
        return;
    }
    while(bRunning)
    {
        struct sockaddr_in clientaddr{0};
        socklen_t addrlen = sizeof(clientaddr);
        std::cout << "ModbusServer::run__: calling accept" << "\n";
        int newSock = accept(server_socket_, (struct sockaddr *) &clientaddr, &addrlen);
        if (newSock != -1)
        {
            std::cout << "ModbusServer::run__: newSock = " << newSock << "\n";
            auto newConn = new ModbusConnection(newSock, clientaddr, this);
            registerConnection(newConn);
            newConn->run();
        }
    }
}

void ModbusServer::stop__()
{
    std::cout << "ModbusServer::stop__" << "\n";
    close(server_socket_);
    cancelIO();
    StopConnections();
}

void ModbusServer::StopConnections()
{
    std::cout << "ModbusServer::StopConnections" << "\n";
    for (auto conn:connections_)
    {
        conn->stop();
    }
    connections_.clear();
}

void ModbusServer::registerConnection(Worker *w)
{
    std::cout << "ModbusServer::registerConnection" << "\n";
    std::lock_guard<std::mutex> guard(connsetMutex);
    connections_.insert(w);
    std::cout << "ModbusServer::registerConnection: connections_.size() = " << connections_.size() << "\n";
}

void ModbusServer::deregisterConnection(Worker *w)
{
    std::cout << "ModbusServer::registerConnection" << "\n";
    std::lock_guard<std::mutex> guard(connsetMutex);
    connections_.erase(w);
    server_->deleteWorker(w);
    std::cout << "ModbusServer::registerConnection: connections_.size() = " << connections_.size() << "\n";
}

int ModbusServer::handleModbusRequest(int sock)
{
    std::cout << "ModbusServer::handleModbusRequest" << "\n";
    std::lock_guard<std::mutex> guard(ctxMutex);
    modbus_set_socket(ctx_, sock);
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int rc = modbus_receive(ctx_, query);
    std::cout << "ModbusServer::handleModbusRequest: after modbus_receive: rc = " << rc << "\n";
    if (rc > 0)
    {
        int n = modbus_reply(ctx_, query, rc, mb_mapping_);
        std::cout << "ModbusServer::handleModbusRequest: after modbus_reply: n = " << n << "\n";
    }
    return rc;
}
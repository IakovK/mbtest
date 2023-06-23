#pragma once

#include "Worker.h"

#include <mutex>
#include <set>

#include <modbus.h>

class ModbusServer: public Worker, public IModbusService
{
    int port_;
    void run__() override;
    void stop__() override;

    modbus_t *ctx_{nullptr};
    modbus_mapping_t *mb_mapping_{nullptr};
    std::mutex ctxMutex;

    std::set<Worker*> connections_;
    std::mutex connsetMutex;
    IGlobalService *server_;

    int server_socket_{-1};
    void StopConnections();

public:
    ModbusServer(int port, IGlobalService *server);
    ~ModbusServer();
    void registerConnection(Worker *w) override;
    void deregisterConnection(Worker *w) override;
    int handleModbusRequest(int sock) override;
};

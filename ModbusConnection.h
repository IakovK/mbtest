#pragma once

#include "Worker.h"

#include <netinet/in.h>
class ModbusConnection: public Worker
{
    int socket_;
    sockaddr_in clientaddr_;
    IModbusService *server_;
    void run__() override;
    void stop__() override;
public:
    ModbusConnection(int socket, sockaddr_in clientaddr, IModbusService *server);
    ~ModbusConnection();
};

#pragma once

#include "Worker.h"

#include <netinet/in.h>
class ModbusConnection: public Worker
{
    int socket_;
    sockaddr_in clientaddr_;
    IBaseService *server1_;
    IModbusService *server2_;
    void run__() override;
    void stop__() override;
public:
    ModbusConnection(int socket, sockaddr_in clientaddr, IBaseService *server1, IModbusService *server2);
    ~ModbusConnection();
};

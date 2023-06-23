#include "ModbusConnection.h"

#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <fcntl.h>

ModbusConnection::ModbusConnection(int socket, sockaddr_in clientaddr, IModbusService *server)
:socket_(socket)
,clientaddr_(clientaddr)
,server_(server)
{
}

ModbusConnection::~ModbusConnection()
{
}

void ModbusConnection::run__()
{
    std::cout << "New connection from " << inet_ntoa(clientaddr_.sin_addr) << ":" << clientaddr_.sin_port << "\n";
    // making socket nonblocking
    fcntl(socket_, F_SETFL, fcntl(socket_, F_GETFL, 0) | O_NONBLOCK);
    while(bRunning)
    {
        fd_set rdset;
        FD_ZERO(&rdset);
        FD_SET(socket_, &rdset);
        int n = select(socket_ + 1, &rdset, NULL, NULL, NULL);
        if (FD_ISSET(socket_, &rdset))
        {
            int n1 = server_->handleModbusRequest(socket_);
            if (n1 < 0)
                break;
        }
    }
    std::cout << "Closed connection from " << inet_ntoa(clientaddr_.sin_addr) << ":" << clientaddr_.sin_port << "\n";
    server_->deregisterConnection(this);
}

void ModbusConnection::stop__()
{
    close(socket_);
    cancelIO();
}

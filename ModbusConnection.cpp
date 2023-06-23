#include "ModbusConnection.h"

#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

ModbusConnection::ModbusConnection(int socket, sockaddr_in clientaddr, IModbusService *server)
:socket_(socket)
,clientaddr_(clientaddr)
,server_(server)
{
    std::cout << "ModbusConnection::ModbusConnection" << "\n";
}

ModbusConnection::~ModbusConnection()
{
    std::cout << "ModbusConnection::~ModbusConnection" << "\n";
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
        std::cout << "ModbusConnection::run__: calling select" << "\n";
        int n = select(socket_ + 1, &rdset, NULL, NULL, NULL);
        std::cout << "ModbusConnection::run__: calling select completed: n = " << n << "\n";
        if (FD_ISSET(socket_, &rdset))
        {
            int n1 = server_->handleModbusRequest(socket_);
            std::cout << "ModbusConnection::run__: after handleModbusRequest(socket_): n1 = " << n1 << "\n";
            if (n1 < 0)
                break;
        }
    }
    std::cout << "ModbusConnection::run__: exiting" << "\n";
    std::cout << "Closed connection from " << inet_ntoa(clientaddr_.sin_addr) << ":" << clientaddr_.sin_port << "\n";
    server_->deregisterConnection(this);
}

void ModbusConnection::stop__()
{
    std::cout << "ModbusConnection::stop__" << "\n";
    close(socket_);
    cancelIO();
}

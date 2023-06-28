#pragma once

#include <boost/asio.hpp>
#include "Worker.h"
#include "BaseServer.h"

class TcpServer: public Worker, public BaseServer
{
    int port_;
    IModbusService *server2_;
    void run__() override;
    void stop__() override;

    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;

    public:
        TcpServer(int port, IGlobalService *server1, IModbusService *server2);
        ~TcpServer();
};

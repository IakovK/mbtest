#include <iostream>
#include "TcpServer.h"
#include "TcpConnection.h"

TcpServer::TcpServer(int port, IGlobalService *server1, IModbusService *server2)
:port_(port)
,acceptor_(io_context_)
,BaseServer(server1)
,server2_(server2)
{
    std::cout << "TcpServer::TcpServer: port = " << port << "\n";
}

TcpServer::~TcpServer()
{
}

void TcpServer::run__()
{
    boost::system::error_code ec;
    acceptor_.open(boost::asio::ip::tcp::v4(), ec);
    if (ec)
    {
        std::cout << "failed to open listening socket. exiting" << "\n";
        return;
    }
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port_), ec);
    if (ec)
    {
        std::cout << "failed to bind listening socket. exiting" << "\n";
        return;
    }
    acceptor_.listen(boost::asio::socket_base::max_connections, ec);
    if (ec)
    {
        std::cout << "failed to start listening . exiting" << "\n";
        return;
    }
    while(bRunning)
    {
        boost::asio::ip::tcp::socket socket(io_context_);
        acceptor_.accept(socket, ec);
        if (!ec)
        {
            auto newConn = new TcpConnection(std::move(socket), this, server2_);
            registerConnection(newConn);
            newConn->run();
        }
    }
}

void TcpServer::stop__()
{
    acceptor_.close();
    cancelIO();
    stopConnections();
}

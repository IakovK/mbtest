#include "TcpConnection.h"
#include "protocol.h"

#include <iostream>

TcpConnection::TcpConnection(boost::asio::ip::tcp::socket &&socket, IBaseService *server1, IModbusService *server2)
:socket_(std::move(socket))
,server1_(server1)
,server2_(server2)
{
}

TcpConnection::~TcpConnection()
{
}

void TcpConnection::run__()
{
    auto ep = socket_.remote_endpoint();
    std::cout << "New connection from " << ep  << "\n";
    boost::system::error_code ec;
    while(bRunning)
    {
        std::uint8_t cmd{0};
        std::uint16_t arg{0};
        size_t rl1 = boost::asio::read(socket_, boost::asio::buffer(&cmd, 1), ec);
        size_t rl2 = boost::asio::read(socket_, boost::asio::buffer(&arg, 2), ec);
        if (ec)
            break;
        arg = ntohs(arg);
        switch(cmd)
        {
            case READ_COIL:
                readCoil(arg);break;
            case READ_DISCRETE:
                readDiscrete(arg);break;
            case READ_INPUT:
                readInput(arg);break;
            case READ_HOLDING:
                readHolding(arg);break;
            case READ_COIL_ALL:
                readCoilAll();break;
            case READ_DISCRETE_ALL:
                readDiscreteAll();break;
            case READ_INPUT_ALL:
                readInputAll();break;
            case READ_HOLDING_ALL:
                readHoldingAll();break;
            case READ_ALL:
                readAll();break;
            case START_LOOP:
                startLoop();break;
            case STOP_LOOP:
                stopLoop();break;
            default:
                break;
        }
    }
    std::cout << "Closed connection from " << ep << "\n";
    server1_->deregisterConnection(this);
}

void TcpConnection::stop__()
{
    boost::system::error_code ec;
    socket_.close(ec);
    cancelIO();
}

void TcpConnection::WriteResponse(uint8_t *data, uint32_t size)
{
    boost::system::error_code ec;
    uint32_t size1 = ntohl(size);
    boost::asio::write(socket_, boost::asio::buffer(&size1, 4), ec);
    if (size != 0)
    {
        boost::asio::write(socket_, boost::asio::buffer(data, size), ec);
    }
}

void TcpConnection::readCoil(int arg)
{
    uint8_t retVal{0};
    int n = server2_->readCoil(arg, retVal);
    if (n > 0)
    {
        WriteResponse(&retVal, 1);
    }
    else
    {
        WriteResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readDiscrete(int arg)
{
    uint8_t retVal{0};
    int n = server2_->readDiscrete(arg, retVal);
    if (n > 0)
    {
        WriteResponse(&retVal, 1);
    }
    else
    {
        WriteResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readInput(int arg)
{
    uint16_t retVal{0};
    int n = server2_->readInput(arg, retVal);
    if (n > 0)
    {
        retVal = htons(retVal);
        WriteResponse((uint8_t*)&retVal, 2);
    }
    else
    {
        WriteResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readHolding(int arg)
{
    uint16_t retVal{0};
    int n = server2_->readHolding(arg, retVal);
    if (n > 0)
    {
        retVal = htons(retVal);
        WriteResponse((uint8_t*)&retVal, 2);
    }
    else
    {
        WriteResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readCoilAll()
{
    std::cout << "TcpConnection::readCoilAll" << "\n";
}

void TcpConnection::readDiscreteAll()
{
    std::cout << "TcpConnection::readDiscreteAll" << "\n";
}

void TcpConnection::readInputAll()
{
    std::cout << "TcpConnection::readInputAll" << "\n";
}

void TcpConnection::readHoldingAll()
{
    std::cout << "TcpConnection::readHoldingAll" << "\n";
}

void TcpConnection::readAll()
{
    std::cout << "TcpConnection::readAll" << "\n";
}

void TcpConnection::startLoop()
{
    std::cout << "TcpConnection::startLoop" << "\n";
}

void TcpConnection::stopLoop()
{
    std::cout << "TcpConnection::stopLoop" << "\n";
}

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

void TcpConnection::writeResponse(uint8_t *data, uint32_t size)
{
    boost::system::error_code ec;
    uint32_t size1 = ntohl(size);
    boost::asio::write(socket_, boost::asio::buffer(&size1, 4), ec);
    if (size != 0)
    {
        boost::asio::write(socket_, boost::asio::buffer(data, size), ec);
    }
}

void TcpConnection::writeResponse(uint8_t data)
{
    writeResponse(&data, 1);
}

void TcpConnection::writeResponse(uint16_t data)
{
    uint16_t data1 = htons(data);
    std::cout << "TcpConnection::writeResponse(" << data << "), data1 = " << (uint)data1 << "\n";
    writeResponse((uint8_t*)&data1, 2);
}

void TcpConnection::writeResponse(const std::vector<uint8_t> &data)
{
    boost::system::error_code ec;
    uint32_t size = ntohl(data.size());
    boost::asio::write(socket_, boost::asio::buffer(&size, 4), ec);
    if (data.size() != 0)
    {
        boost::asio::write(socket_, boost::asio::buffer(data), ec);
    }
}

void TcpConnection::writeResponse(const std::vector<uint16_t> &data)
{
    boost::system::error_code ec;
    uint32_t size = ntohl(data.size() * 2);
    boost::asio::write(socket_, boost::asio::buffer(&size, 4), ec);
    for (auto w:data)
    {
        uint16_t w1 = htons(w);
        boost::asio::write(socket_, boost::asio::buffer(&w1, 2), ec);
    }
}

void TcpConnection::readCoil(int arg)
{
    uint8_t retVal{0};
    int n = server2_->readCoil(arg, retVal);
    if (n > 0)
    {
        std::cout << "TcpConnection::readCoil(" << arg << ") = " << (uint)retVal << "\n";
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readDiscrete(int arg)
{
    uint8_t retVal{0};
    int n = server2_->readDiscrete(arg, retVal);
    if (n > 0)
    {
        std::cout << "TcpConnection::readDiscrete(" << arg << ") = " << (uint)retVal << "\n";
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readInput(int arg)
{
    uint16_t retVal{0};
    int n = server2_->readInput(arg, retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readHolding(int arg)
{
    uint16_t retVal{0};
    int n = server2_->readHolding(arg, retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readCoilAll()
{
    std::vector<uint8_t> retVal;
    int n = server2_->readCoilAll(retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readDiscreteAll()
{
    std::vector<uint8_t> retVal;
    int n = server2_->readDiscreteAll(retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readInputAll()
{
    std::vector<uint16_t > retVal;
    int n = server2_->readInputAll(retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readHoldingAll()
{
    std::vector<uint16_t > retVal;
    int n = server2_->readHoldingAll(retVal);
    if (n > 0)
    {
        writeResponse(retVal);
    }
    else
    {
        writeResponse(nullptr, 0);  // no response
    }
}

void TcpConnection::readAll()
{
    std::cout << "TcpConnection::readAll" << "\n";
    writeResponse(nullptr, 0);  // no response
}

#pragma once

#include <boost/asio.hpp>
#include "Worker.h"

class TcpConnection: public Worker
{
    enum { max_length = 1024 };
    boost::asio::ip::tcp::socket socket_;
    IBaseService *server1_;
    IModbusService *server2_;
    void run__() override;
    void stop__() override;
    // command handlers
    void readCoil(int arg);
    void readDiscrete(int arg);
    void readInput(int arg);
    void readHolding(int arg);
    void readCoilAll();
    void readDiscreteAll();
    void readInputAll();
    void readHoldingAll();
    void readAll();
    void writeResponse(uint8_t *data, uint32_t size);
    void writeResponse(uint8_t data);
    void writeResponse(uint16_t data);
    void writeResponse(const std::vector<uint8_t> &data);
    void writeResponse(const std::vector<uint16_t> &data);
public:
    TcpConnection(boost::asio::ip::tcp::socket &&socket, IBaseService *server1, IModbusService *server2);
    ~TcpConnection();
};

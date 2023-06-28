#pragma once

#include <cstdint>
#include <vector>

class IBaseService
{
public:
    virtual void registerConnection(class Worker *w) = 0;
    virtual void deregisterConnection(class Worker *w) = 0;
};

class IModbusService
{
public:
    virtual int handleModbusRequest(int sock) = 0;
    virtual int readCoil(int arg, std::uint8_t &val) = 0;
    virtual int readInput(int arg, std::uint16_t &val) = 0;
    virtual int readDiscrete(int arg, std::uint8_t &val) = 0;
    virtual int readHolding(int arg, std::uint16_t &val) = 0;
    virtual int readCoilAll(std::vector<uint8_t> &val) = 0;
    virtual int readInputAll(std::vector<uint16_t> &val) = 0;
    virtual int readDiscreteAll(std::vector<uint8_t> &val) = 0;
    virtual int readHoldingAll(std::vector<uint16_t> &val) = 0;
};

class IGlobalService
{
public:
    virtual void deleteWorker(class Worker *w) = 0;
};

#pragma once

class IModbusService
{
public:
    virtual void registerConnection(class Worker *w) = 0;
    virtual void deregisterConnection(class Worker *w) = 0;
    virtual int handleModbusRequest(int sock) = 0;
};

class IGlobalService
{
public:
    virtual void deleteWorker(class Worker *w) = 0;
};

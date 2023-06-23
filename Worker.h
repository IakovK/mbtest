#pragma once

#include "Interfaces.h"

#include <thread>

class Worker
{
    static void run_(Worker *this_);
protected:
    bool bRunning{false};
    std::thread thread_;
    virtual void run__() = 0;
    virtual void stop__() = 0;
    void cancelIO();
public:
    void run();
    void stop();
    void wait();
    virtual ~Worker(){}
};

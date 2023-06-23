#pragma once

#include "Worker.h"
#include "Interfaces.h"

class ThreadDeleter : public Worker, public IGlobalService
{
public:
    void run__() override;
    void stop__() override;
    void deleteWorker(Worker *w) override;
};

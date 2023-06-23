#pragma once

#include "Worker.h"
#include "Interfaces.h"
#include "BlockingCollection.h"

class ThreadDeleter : public Worker, public IGlobalService
{
    code_machina::BlockingCollection<Worker *> threads_;
public:
    void run__() override;
    void stop__() override;
    void deleteWorker(Worker *w) override;
};

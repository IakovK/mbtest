#include "ThreadDeleter.h"

#include <iostream>

void ThreadDeleter::run__()
{
    std::cout << "ThreadDeleter::run__" << "\n";
}

void ThreadDeleter::stop__()
{
    std::cout << "ThreadDeleter::stop__" << "\n";
}

void ThreadDeleter::deleteWorker(Worker *w)
{
    std::cout << "ThreadDeleter::deleteWorker" << "\n";
}

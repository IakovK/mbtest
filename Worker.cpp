#include "Worker.h"

#include <iostream>
#include <signal.h>

void Worker::run()
{
    bRunning = true;
    thread_ = std::thread(run_, this);
}

void Worker::run_(Worker *this_)
{
    this_->run__();
}

void Worker::stop()
{
    bRunning = false;
    stop__();
}

static void noop(const int sig)
{
    std::cout << "noop" << "\n";
}

void Worker::cancelIO()
{
    std::cout << "ModbusServer::cancelIO: entry" << "\n";
    struct sigaction newAction{0};
    (void)sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = noop;
    int n1 = sigaction(SIGUSR1, &newAction, NULL);
    std::cout << "ModbusServer::cancelIO: sigaction(SIGUSR1) returned " << n1 << "\n";
    int n2 = pthread_kill(thread_.native_handle(), SIGUSR1); // system calls return with EINTR
    std::cout << "ModbusServer::cancelIO: pthread_kill(SIGUSR1) returned " << n2 << "\n";
    std::cout << "ModbusServer::cancelIO: exit" << "\n";
}

void Worker::wait()
{
    std::cout << "Worker::wait: entry" << "\n";
    if (thread_.joinable())
    {
        std::cout << "Worker::wait: thread is joinable. calling join()" << "\n";
        thread_.join();
        std::cout << "Worker::wait: thread is joinable. calling join() complete" << "\n";
    }
    std::cout << "Worker::wait: exit" << "\n";
}

#include "Worker.h"

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
}

void Worker::cancelIO()
{
    struct sigaction newAction{0};
    (void)sigemptyset(&newAction.sa_mask);
    newAction.sa_flags = 0;
    newAction.sa_handler = noop;
    int n1 = sigaction(SIGUSR1, &newAction, NULL);
    int n2 = pthread_kill(thread_.native_handle(), SIGUSR1); // system calls return with EINTR
}

void Worker::wait()
{
    if (thread_.joinable())
    {
        thread_.join();
    }
}

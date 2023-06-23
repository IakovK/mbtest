#include "ThreadDeleter.h"

void ThreadDeleter::run__()
{
    while(!threads_.is_completed())
    {
        Worker* worker;

        // take will block if there is no data to be taken
        auto status = threads_.take(worker);

        if(status == code_machina::BlockingCollectionStatus::Ok)
        {
            worker->wait();
            delete worker;
        }

      // Status can also return BlockingCollectionStatus::Completed meaning take was called
      // on a completed collection. Some other thread can call complete_adding after we pass
      // the is_completed check but before we call take. In this example, we can ignore that
      // status since the loop will break on the next iteration.
    }
}

void ThreadDeleter::stop__()
{
    threads_.complete_adding();
}

void ThreadDeleter::deleteWorker(Worker *w)
{
    threads_.add(w);
}

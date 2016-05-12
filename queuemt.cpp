#include "queuemt.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>

template <typename T>
QueueMt<T>::QueueMt()
{

}

template <typename T>
QueueMt<T>::~QueueMt()
{
    Common::printToConsole("QueueMt::Destructor...");
    if (_t)
    {
        delete _t;
    }
}

template <typename T>
void QueueMt<T>::SetOomSimulation(bool mode)
{
    if ( mode == true )
        std::cout << "QueueMt::setOomSimulation() Exception simulation in QueueMt enabled..." << std::endl;
    OomErrorSimilation = mode;
}

template <typename T>
bool QueueMt<T>::Push(T const& item)
{
    bool retValue = true;
    // Lock the queue
    // this operation is expected to happen very quickly so using a blocking lock here...
    mutex.lock();

    if ( OomErrorSimilation == true )
    {
        std::cout << "QueueMt::Push() Exception simulation..." << std::endl;
        throw std::runtime_error("QueueMT:Push(): FATAL ERROR: Queue is unable to accept item");
    }

    try
    {
        workQueue.push(item);
    }
    catch (...)
    {
        // rather general but catch any STL error. catch here so we can throw an error msg
        // in future could also add retry if the error is recoverable.
        // fatal error adding item...
        throw std::runtime_error("QueueMT:Push(): FATAL ERROR: Queue is unable to accept item");
    }

    mutex.unlock();

    // new item added successfully
    if ( waitingForWakeup == true )
    {
        // a previous call to POP() is waiting for a new item.. wake up here...
        waitingForWakeup = false;
        std::unique_lock<std::mutex> lck(popLock);
        popCondLock.notify_all();
        //Common::printToConsole("QueueMT: waking POP() call...");
    }

    return retValue;
}

template <typename T>
T QueueMt<T>::Pop()
{
    // Assumption: only called by one consumer (worker thread)!
    // will lock current thread until an item becomes available..
    T retValue;

    // Lock the queue
    // this operation is expected to happen very quickly so using a blocking lock here...
    mutex.lock();
    if ( workQueue.empty() == true )
    {
        // Queue is currently empty, unlock the queue and wait on a event from push()
        mutex.unlock();

        // wait
        Common::printToConsole("QueueMT: going to sleep as there are no items available to return...");
        waitingForWakeup = true;
        std::unique_lock<std::mutex> lck(popLock);

        // wait for push() generated event
        popCondLock.wait(lck);
        Common::printToConsole("QueueMT: waking up item available...");

        // Do a sanity check..
        if ( workQueue.empty() == true )
        {
            mutex.unlock();
            throw std::runtime_error("QueueMT:Pop(): FATAL SYNC WAKE ERROR: Queue is empty");
        }
    }

    retValue = workQueue.front();

    // remove item from queue...
    workQueue.pop();

    // unlock queue mutex...
    mutex.unlock();

    return retValue;
}

template <typename T>
T QueueMt<T>::Pop_try()
{
    // Lock the queue
    // this operation is expected to happen very quickly so using a blocking lock here...
    T retValue;
    mutex.lock();

    if ( workQueue.empty() == true )
    {
        // queue is empty so throw an exception.
        mutex.unlock();
        throw std::runtime_error("QueueMT:Pop(): Queue is empty");
    }

    retValue = workQueue.front();
     // remove item from queue...
    workQueue.pop();

    // unlock queue mutex...
    mutex.unlock();
    return retValue;
}

template class QueueMt<Common::workPackageStruct>;

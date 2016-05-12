#ifndef _QUEUE_MT_H
#define _QUEUE_MT_H

#include "common.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

using namespace std;

/**
 * @class QueueMt
 * @author JThompson
 * @date 12/05/16
 * @file queuemt.h
 * @brief template class wrapper for std::queue.  Thread safe for one writer/one reader
 */
template <typename T>
class QueueMt
{
public:
    QueueMt<T>();
    ~QueueMt<T>();

    /**
     * @return  true : item added
     *          false: item not added to queue
     * @brief   The writer thread calls this function to push an item into the queue. If there is no room
                in the queue for the item, the function should return immediately and indicate to the
                calling thread that the item was not pushed to the queue.
     */
    bool Push(T const&);

    /**
     * @brief   The reader thread calls this function to fetch the next available item. If no items are
                available, the background thread should be put to sleep until an item becomes
                available.
     * @return  <T> next available item, exception thrown on no data
     * @exception
     */
    T Pop();

    /**
     * @brief   The reader thread calls this function to fetch the next available item. If no items are
                available, the function will return immediately and indicate to the calling thread that no
                items are available
     */
    T Pop_try();

    /**
     * @brief Simulate an error in Push()
     *        Note: dirty way to do this not ideal.
     * @param mode Mode to use
     */
    void SetOomSimulation(bool mode);


private:
    thread*             _t;
    condition_variable  _cond;
    std::mutex          mutex;

    bool                OomErrorSimilation  = false;    // will trigger a simulated error in Push() if enabled
    bool                waitingForWakeup    = false;
    std::mutex          popLock;
    std::condition_variable popCondLock;

    unsigned int        _number;
    int                 _clientSock;

    // Internal storage
    queue<T>            workQueue;
};

#endif

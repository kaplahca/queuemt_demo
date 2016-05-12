#ifndef _WORKER_H
#define _WORKER_H

#include "queuemt.h"
#include "common.h"

#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

/**
 * @class WORKER
 * @author jim
 * @date 08/05/16
 * @file hdr.h
 * @brief internal header class
 */
class Worker
{
    public:
        Worker(QueueMt<Common::workPackageStruct>* workQueue);
        ~Worker();
        Worker(const Worker&) = delete;


        /**
         * @brief create and start the worker thread @see ThreadMain()
         */
        void StartThread();
        
        /**
         * @brief join() the writer thread and wait for completion.
         */
        void WaitForCompletion();

    private:
        void ThreadMain();

private:
        // Made private as object should not be instantiated this way.
        Worker();
        const char* threadName = "WORKer";
        std::thread threadHandle;
        int     stuff   = 0;

        // handle to external resource...
        QueueMt<Common::workPackageStruct>* extQueue;
};

#endif

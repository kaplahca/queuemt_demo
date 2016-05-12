#ifndef _SERVER_H
#define _SERVER_H

#include "common.h"
#include "queuemt.h"

#include <thread>
#include <mutex>
#include <condition_variable>


using namespace std;

/**
 * @class Server
 * @author jim
 * @date 08/05/16
 * @file hdr.h
 * @brief internal header class
 */
class Writer
{
    public:
        Writer(QueueMt<Common::workPackageStruct>* workQueue);
        ~Writer();
        Writer(const Writer&) = delete;

        /**
         * @brief create and start the writer thread @see ThreadMain()
         * @param demoMode
         * @param noTestPackets
         * @return thread execution status.
        */
        bool StartThread(Common::E_DEMO_MODE demoMode, int noTestPackets);
        
        /**
         * @brief join() the writer thread and wait for completion.
         */
        void WaitForCompletion();

    private:
        // Made private as object should not be instantiated this way.
        Writer();
        void ThreadMain();

    private:
        const char* threadName          = "Writer";
        Common::E_DEMO_MODE mode        = Common::INVALID_STATE;

        // Thread/event bits
        std::thread         threadHandle;
        condition_variable  _cond;
        mutex               _mtx;

        // test stuff
        int                 noTestCylces = 3;

        // handle to external resource...
        QueueMt<Common::workPackageStruct>* extQueue;
};

#endif

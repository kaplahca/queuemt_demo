#ifndef _MCU_H
#define _MCU_H

#include "queuemt.h"
#include "writer.h"
#include "worker.h"

#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

/**
 * @class Mcu
 * @author jim
 * @date 08/05/16
 * @file hdr.h
 * @brief main controller class for thread/FIFO queue demo.
 */
class Mcu
{
    public:
        Mcu();
        ~Mcu();
        Mcu(const Mcu&) = delete;

        /**
         * @brief
         * @param demoMode      execution mode for the demo @see Mcu::E_DEMO_MODE
         * @param noWorkItems   number of packets to be generated by the writer (producer)
         */
        void StartDemo(Common::E_DEMO_MODE demoMode, int noWorkItems);

        /**
         * @brief Execute the demo and wait for server/worker threads to both complete (blocks main app thread)
         */
        void WaitForDemoCompletion();

private:
        /**
         * @brief Start the writer (producer) thread (higher priority)
         * @param noTestPackets Number of test packets to be generated by the writer thread
         */
        void StartWriterThread(Common::E_DEMO_MODE demoMode, int noTestPackets);

        /**
         * @brief Start the worker (consumer) thread, normal priority
         */
        void StartWorkerThread();

        int     stuff   = 0;
        Writer* writer;
        Worker* worker;

        // handle to external resource...
        // Owned by this object.
        QueueMt<Common::workPackageStruct>* workQueue;
};

#endif
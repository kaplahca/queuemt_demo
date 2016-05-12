#include "worker.h"

#include <iostream>
#include <sys/types.h>

/**
 * @brief default constructor
 * @return
 */
Worker::Worker(QueueMt<Common::workPackageStruct>* workQueue)
{
    extQueue = workQueue;
}

/**
 * @brief destructor
 * @return
 */
Worker::~Worker()
{
    Common::printToConsole("Worker::Destructor...");
}

//
// +- Public methods -+
//

/**
 * @brief
 */
void Worker::StartThread()
{
    Common::printToConsoleThread(threadName, "starting thread");

    threadHandle = std::thread(&Worker::ThreadMain, this);

    //cout << "" << endl;
}


void Worker::WaitForCompletion()
{
    // wait for thread to complete...
    Common::printToConsole("Waiting for worker thread to complete...");
    threadHandle.join();
}

//
// +- Private methods -+
//

/**
 * @brief
 */
void Worker::ThreadMain()
{
    std::string msg = "";
    Common::printToConsoleThread(threadName, "thread procesing loop START...");
    // wait for a bit..
    //std::this_thread::sleep_for (std::chrono::seconds(1));

    // Worker thread continues until program exits..

    while (true)
    {
        // try to get a new item from the Queue
        Common::workPackageStruct workItem = extQueue->Pop();

        // Process this item...
        msg = "Worker.. found work item: ID: ";
        msg += std::to_string(workItem.id);
        msg += " work to do (ms):";
        msg += std::to_string(workItem.workForMs);
        msg += " .. procssing...";
        Common::printToConsoleThread(threadName, msg.c_str());

        std::this_thread::sleep_for (std::chrono::milliseconds(workItem.workForMs) );
        msg = "";
        msg += "finished procesing (consumed) work item ID: ";
        msg += std::to_string(workItem.id);
        Common::printToConsoleThread(threadName, msg.c_str());

        if ( workItem.lastItem == true )
        {
            Common::printToConsoleThread(threadName, "LAST packet found... (terminate signal)");
            break; // break loop and allow thread to die...
        }
    }


    Common::printToConsoleThread(threadName, "thread procesing loop END...");
}


#include "mcu.h"
#include "common.h"

#include <iostream>
#include <sys/types.h>

/**
 * @brief default constructor
 * @return
 */
Mcu::Mcu()
{
    Common::printToConsole("Mcu::Constuctor...");

    Common::printToConsole("Mcu::Constuctor... queue...");
    workQueue = new QueueMt<Common::workPackageStruct>();

    Common::printToConsole("Mcu::Constuctor... threads...");
    worker = new Worker(workQueue);
    writer = new Writer(workQueue);
}

/**
 * @brief destructor
 * @return
 */
Mcu::~Mcu()
{
    Common::printToConsole("Mcu::Destructor...");
    delete writer;
    delete worker;
    delete workQueue;
}

//
// +- Public methods -+
//

/**
 * @brief
 */
void Mcu::StartDemo(Common::E_DEMO_MODE demoMode, int noWorkItems)
{
    if ( demoMode == Common::DEMO_MODE_OOM_EXCEPTION )
    {
        workQueue->SetOomSimulation(true);
    }

    std::cout << "FIFO demo> .. starting writer (producer) thread..." << std::endl;
    StartWriterThread(demoMode, noWorkItems);

    std::cout << "FIFO demo> .. starting worker (consumer) thread..." << std::endl;
    StartWorkerThread();
}

/**
 * @brief wait for worker/Writer threads to complete...
 */
void Mcu::WaitForDemoCompletion()
{
    // Could use a signal but for simplicity wait on both threads to complete
    Common::printToConsole("Waiting on writer/worker threads to complete...");

    writer->WaitForCompletion();
    worker->WaitForCompletion();
}


//
// +- Private methods -+
//
/**
 * @brief
 */
void Mcu::StartWriterThread(Common::E_DEMO_MODE demoMode, int noTestPackets = 10)
{
    writer->StartThread(demoMode, noTestPackets);
}

/**
 * @brief
 */
void Mcu::StartWorkerThread()
{
    Common::printToConsole("Mcu::Worker starting...");
    worker->StartThread();
}




#include "writer.h"

#include <iostream>
#include <chrono>
#include <random>
#include <sys/types.h>

/**
 * @brief default constructor
 * @return
 */
Writer::Writer(QueueMt<Common::workPackageStruct>* workQueue)
{
    extQueue = workQueue;
}

/**
 * @brief destructor
 * @return
 */
Writer::~Writer()
{
    Common::printToConsole("Writer::Destructor...");
}

//
// +- Public methods -+
//

bool Writer::StartThread(Common::E_DEMO_MODE demoMode, int noTestPackets)
{
    std::string msg = "";
    mode = demoMode;

    if ( noTestPackets <=0 )
    {
        cerr << "erver::StartThread(): FATALERROR: noTestPackets <=0";
        throw std::runtime_error("Writer::StartThread(): FATALERROR: noTestPackets <=0");
    }

    msg = "generating ";
    msg+= std::to_string(noTestPackets);
    msg+= " packets..";
    Common::printToConsole(msg.c_str());

    noTestCylces = noTestPackets;

    threadHandle = std::thread(&Writer::ThreadMain, this);

    // Set thread priority to high (linux only)
    sched_param sparam;
    int         policy;
    pthread_getschedparam(threadHandle.native_handle(), &policy, &sparam);
    sparam.sched_priority = 15;
    if(pthread_setschedparam(threadHandle.  native_handle(), SCHED_FIFO, &sparam))
        Common::printToConsole("unable to set Writer thread to high Priority..");
    else
        Common::printToConsole("Writer thread to high Priority..");
}

void Writer::WaitForCompletion()
{
    // wait for Writer thread to complete...
    Common::printToConsole("Waiting for Writer thread to complete...");
    threadHandle.join();
}

//
// +- Private methods -+
//

void Writer::ThreadMain()
{
    Common::printToConsoleThread(threadName, "thread procesing loop START...");
    std::this_thread::sleep_for (std::chrono::milliseconds(1000) );
    int randTime    = 0;
    int id          = 0;
    std::string msg = "";

    // start
    std::random_device rd; // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator

    // setup random distribution ranges based on the demo mode
    // this controls the amount of 'work' required by the writer/worker per packet to simulate work loads
    std::uniform_int_distribution<> distr_worker; // define the rand range
    std::uniform_int_distribution<> distr_writer; // define the rand range

    switch ( mode )
    {
        case Common::DEMO_MODE_HIGH_SERV_LOW_WORKER:
            Common::printToConsoleThread(threadName, "using demo mode: DEMO_MODE_HIGH_SERV_LOW_WORKER");
            distr_writer = std::uniform_int_distribution<>(4000,6000); // high load on server per packet
            distr_worker = std::uniform_int_distribution<>(2000,3000); // low load on worker per packet
            break;

        case Common::DEMO_MODE_LOW_SERV_HIGH_WORKER:
            Common::printToConsoleThread(threadName, "using demo mode: DEMO_MODE_LOW_SERV_HIGH_WORKER");
            distr_writer = std::uniform_int_distribution<>(2000,3000); // low load on server per packet
            distr_worker = std::uniform_int_distribution<>(4000,6000); // high load on writer per packet
            break;

        case Common::DEMO_MODE_EVEN:
            Common::printToConsoleThread(threadName, "using demo mode: DEMO_MODE_EVEN");
            distr_writer = std::uniform_int_distribution<>(2000,6000); // even chance of load on both threads
            distr_worker = std::uniform_int_distribution<>(2000,6000);
            break;

        case Common::DEMO_MODE_OOM_EXCEPTION:
            distr_writer = std::uniform_int_distribution<>(2000,6000); // even chance of load on both threads
            distr_worker = std::uniform_int_distribution<>(2000,6000);
            Common::printToConsoleThread(threadName, "using demo mode: DEMO_MODE_OOM_EXCEPTION");
            break;

        default:
            // opps...
            // should be caught by Main but just in case...
            msg = "Writer::ThreadMain() FATAL ERROR: invalid demo mode: ";
            msg += std::to_string(mode);
            Common::printToConsoleThread(threadName, msg.c_str());
            throw std::runtime_error(msg);
            break;
    }

    //
    // 'Server' run through test cycle generating packets and 'processing' between each
    for  ( int ct = 0 ; ct < noTestCylces ; ct++, id++ )
    {
        // wait for a bit..
        randTime = distr_writer(eng);
        msg = "Writer.. sleep (simulate processing) for ";
        msg += std::to_string(randTime);
        msg += "ms";
        Common::printToConsoleThread(threadName, msg.c_str());
        std::this_thread::sleep_for (std::chrono::milliseconds(randTime) );

        Common::workPackageStruct workItem;
        workItem.id         = id;
        workItem.lastItem   = false;
        workItem.workForMs  = distr_worker(eng); // use a new rand to better simulate an uneven  workflow

        msg = "adding work item to queue..";
        msg+= "ID: ";
        msg+= std::to_string(id);
        msg+= " amount of work: ";
        msg+= std::to_string(workItem.workForMs);
        Common::printToConsoleThread(threadName, msg.c_str());

        extQueue->Push(workItem);
    }

    // Add one last time to the queue, on processing this item the worker thread will terminate causing the program
    // to exit
    // Note: In a real system a signal could be sent by the Writer/worker to indicate end-of-life
    Common::workPackageStruct workItem;
    workItem.id         = id++;
    workItem.lastItem   = true;
    workItem.workForMs  = distr_worker(eng);
    extQueue->Push(workItem);


    Common::printToConsoleThread(threadName, "thread procesing loop END...");
}



#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <ctime>
using namespace std;

/**
 * @class Common
 * @author JThompson
 * @date 12/05/16
 * @file common.h
 * @brief Common collection of utlity + data structures for MCU FIFO demo.
 */
class Common
{
    public:
        enum E_DEMO_MODE
            {
                DEMO_MODE_HIGH_SERV_LOW_WORKER,
                DEMO_MODE_LOW_SERV_HIGH_WORKER,
                DEMO_MODE_EVEN,
                DEMO_MODE_OOM_EXCEPTION,
                INVALID_STATE
            };

    public:
        static void printToConsole(const char* string)
            {
                // Note string is passed in a single const& this should limit output errors if multiple
                // threads call concurrently
                // TODO: move to a thread safe stream solution.
                cout << string << endl;
            }

        static void printToConsoleThread(const char* threadName, const char* string)
            {
                // Note not thread safe, if multiple threads call concurrently output may be currupted
                // TODO: move to a thread safe stream solution...

                // Block from http://en.cppreference.com/w/cpp/chrono/c/strftime
                time_t     now  = time(0);
                struct tm  tstruct;
                char       buf[80];
                tstruct =  *localtime(&now);
                strftime(buf, sizeof(buf), "%X: ", &tstruct);
                // End block

                cout << buf << threadName << ": " << string << endl;
            }


public:
    
    /**
     * @class workPackageStruct
     * @author JThompson
     * @date 12/05/16
     * @file common.h
     * @brief main work package structure used by worker/reader threads. stored in instance
     *        of QueueMt<T>  
     */
        struct workPackageStruct
            {
                int     id;         // UID for this work item (debugging not used for logic)
                int     workForMs;  // example work item will pause the worker thread by specified ms
                bool    lastItem;   // indicates the last time in the work stream, terminate thread after this item.
            };
};

#endif

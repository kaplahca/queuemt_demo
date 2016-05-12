#include "mcu.h"
#include "common.h"

#include <thread>
#include <iostream>
#include <string>

// forward declarations
Common::E_DEMO_MODE menuSystem();

int main(int argc, char* argv[])
{
    int retValue                    = 0;
    Mcu* mcuController              = NULL;
    int noWorkItems                 = 0;
    Common::E_DEMO_MODE demoMode    = Common::INVALID_STATE;

    std::cout << "FIFO demo> prepared for MG 2016/JThompson ver 0.1" << endl;

    demoMode = menuSystem();

    if ( demoMode != Common::INVALID_STATE )
    {
        try
        {
            // Get the number of work packets to generate...
            std::cout << "FIFO demo> no work items? (0:default):";
            std::cin  >> noWorkItems;
            if ( noWorkItems <= 0 )
                // set to default
                noWorkItems = 10;
            std::cout << endl;

            mcuController = new Mcu();

            mcuController->StartDemo(demoMode, noWorkItems);

            mcuController->WaitForDemoCompletion();
        }
        catch (std::exception& e)
        {
            std::string errString;
            errString = "fatal std-exception in McuController";
            errString += e.what();

            Common::printToConsole(errString.c_str());
            cerr << errString;

            // tidy and quit...
            retValue = 1;
        }
        catch (...)
        {
            cerr << "Unknown exception in McuController\n";
            // tidy and quit
            retValue = 1;
        }
    } // end of IF valid state...


    if ( mcuController != NULL )
    {
        Common::printToConsole("tidy...");
        delete mcuController;
    }

    Common::printToConsole("program exit. goodbye.");

    return retValue;
}

/**
 * @brief Display a simple menu to the user and allow various execution modes to be selected...
 */
Common::E_DEMO_MODE menuSystem()
{
    Common::E_DEMO_MODE retValue    = Common::INVALID_STATE;
    int input                       = 0;
    bool waitingForInput            = true;

    cout << "FIFO demo> " << "<execution mode>" << endl;
    cout << "FIFO demo> " << ">(1) low intensity of server msgs, worker on average has to wait (wait on msg demo)" << endl;
    cout << "FIFO demo> " << ">(2) high intensity of server msg, worker on avg cannot keep up (large # queued msgs demo)" << endl;
    cout << "FIFO demo> " << ">(3) server/worker activity level roughly equal" << endl;
    cout << "FIFO demo> " << ">(4) simulated OOM execption while adding msg to queue" << endl;
    cout << "FIFO demo> " << ">(5) quit" << endl;

    while (waitingForInput)
    {
        cout << "FIFO demo> :? ";
        cin >> input;
        switch (input)
        {
            case (1):
                retValue        = Common::DEMO_MODE_HIGH_SERV_LOW_WORKER;
                waitingForInput = false;
                break;
            case (2):
                retValue        = Common::DEMO_MODE_LOW_SERV_HIGH_WORKER;
                waitingForInput = false;
                break;
            case (3):
                retValue        = Common::DEMO_MODE_EVEN;
                waitingForInput = false;
                break;
            case (4):
                retValue        = Common::DEMO_MODE_OOM_EXCEPTION;
                waitingForInput = false;
                break;
            case (5):
                retValue        = Common::INVALID_STATE;
                waitingForInput = false;
                break;
            default:
                cout << "unknown input.. valid options (1,2,3,4,5)" << endl;
                break;
        }
    }

    return retValue;
} // END Mcu::E_DEMO_MODE menuSystem()




/* ****************************************************************************
*
* FILE                     ProcessAssistant.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 5 2010
*
*/
#include "ProcessAssistant.h"      // Own interface


namespace ss {



/* ****************************************************************************
*
* Constructor
*/
ProcessAssistant::ProcessAssistant(int coreNo)
{
	// Should create a separate process with a class Process running on it
	// There should be a pipe connecting both the ProcessAssitant and the Process
	// Create a thread running run

	core = coreNo;
}



/* ****************************************************************************
*
* run - 
*/
void ProcessAssistant::run(void)
{
	// TODO: Andreu
		   
	// Get a new task to do ( or block until ready )
		   
	// Pass command to Process with "runCommand"

	// Loop receiving messages from Process until "finish" or "crash" received
	// Send "continue"
}



/* ****************************************************************************
*
* runCommand
*
* This method sends a command to the Process and waits for commands back.
* This method will be called by TaskManager.
* 
* This is supposed to be a blocking method until one of two thing happens:
* - The finish command is received from the Process
* - An error: Proces crasshes, report error or timeout is triggered.
* 
* RETURN VALUE
* true is returned upon success and 
* false is returned on error (any error)
*/
std::string ProcessAssistant::runCommand(std::string, int timeOut)
{
	// Return "crash" // "timeout" or message from Process

	return "Not implemented";
}

}

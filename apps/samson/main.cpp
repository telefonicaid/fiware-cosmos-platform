/* ****************************************************************************
*
* FILE                     main.cpp - main program for samson
*/

 
/* ****************************************************************************
*
*
*/


#include <iostream>				// std::cout ...
#include "network.h"			// NetworkInterface
#include "packet.h"				// Packet
#include "endpoint.h"			// EndPoint
#include "CommandLine.h"		// CommandLine
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss::SamsonWorker


int main(int arg , const char *argv[])
{

	au::CommandLine commandLine;
	commandLine.parse(arg , argv);
	commandLine.set_flag_boolean("controller");	 //	-controller to create the samson controller
	commandLine.parse(arg, argv);

	if( commandLine.get_flag_bool("controller") )
	{
		// Run the controller
		ss::SamsonController controller( arg , argv );
		controller.run();
		
	}
	else
	{
		// Run the worker
		ss::SamsonWorker worker( arg , argv );
		worker.run();

		
		
	}
	
	
	
}

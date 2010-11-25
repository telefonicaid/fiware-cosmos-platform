
#include "Process.h"						// Own interface
#include <iostream>							// std::cout
#include "ProcessOperationFramework.h"		// ss::ProcessOperationFramework

namespace ss {

	network::ProcessMessage Process::runCommand( network::ProcessMessage m)
	{
		
		
		ProcessOperationFramework * framework2;
		
		framework2 = new ProcessOperationFramework(this , m );
		framework2->run();
		delete framework2;
		
		// passCommand( 0 );	// Eventually passing this command
		
		// Rigth now finish without any message
		// TODO: Be able to recover errors from the 3tr party operations
		return processMessageWithCode( network::ProcessMessage::finished );
		
	}

	

}
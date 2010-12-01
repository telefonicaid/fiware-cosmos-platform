#ifndef _H_PROCESS_BASE
#define _H_PROCESS_BASE

#include "ProcessCommunication.h"		// ss::ProcessCommunication
#include "samson.pb.h"					// ss::network:..
#include "logMsg.h"						// Log
#include <iostream>						// std::cout

namespace ss {
	
	class ProcessOperationFramework;	
	
	
	/* ****************************************************************************
	 *
	 * Process - 
	 *
	 * This class is intanciated in the independent process created by any ProcessAssitant.
	 * There will be a pipe connecting ProcessAssitant and Process
	 *
	 * Basically this class blocks the main thread of this independent process in the "run" method until a command is received from the pipe.
	 * once a command is received, runCommand method is executed.
	 * During runCommand executiong, it can eventually be called the passCommand method to sent "messages" to the ProcessAssistant.
	 * This method should be blocking until ProcessAssitant confirms it has received and process the message
	 * Finally, when runCommand has finished a notification message is sent back to the ProcessAssistant to notify that everything has finished
	 * and run methods blocks again waiting for the next command
	 */
	
	class ProcessBase : public ProcessCommunication
	{
		
	protected:
		
		ProcessBase(int rFd, int wFd );
		
	public:
		void run(void);
		
	protected:
		
		// Method to be implemented by top class
		virtual network::ProcessMessage runCommand(network::ProcessMessage m)=0;

		// Read and write functions
		
		void read( network::ProcessMessage &p)
		{
			if ( _read( p ) == -1 )
			{
				LM_X(1, ("father died - I die as well"));
			}
		}
		
		void write( network::ProcessMessage &p)
		{
			if ( _write( p ) == -1 )
			{
				LM_X(1, ("father died - I die as well"));
			}
		}
		
		
		
	public:
		
		// Method to asynchronously ( and blocking) send messages to ProcessAssitant
		network::ProcessMessage passCommand( network::ProcessMessage );
	
		// Handy function to pass a "process output" packet
		void processOutput()
		{
			passCommand( processMessageWithCode( network::ProcessMessage::process_output ) );
		}
		
	};
	
}

#endif
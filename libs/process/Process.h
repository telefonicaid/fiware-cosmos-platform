
#ifndef _H_PROCESS
#define _H_PROCESS

namespace ss {

	/**
	 This class is intanciated in the independent process created by any ProcessAssitant.
	 There will be a pipe connecting ProcessAssitant and Process

	 Basically this class blocks the main thread of this independent process in the "run" method until a command is received from the pipe.
	 once a command is received, runCommand method is executed.
	 During runCommand executiong, it can eventually be called the passCommand method to sent "messages" to the ProcessAssistant.
	 This method should be blocking until ProcessAssitant confirms it has received and process the message
	 Finally, when runCommand has finished a notification message is sent back to the ProcessAssistant to notify that everything has finished
	 and run methods blocks again waiting for the next command
	 */
	
	class Process{

		
		Process()
		{
			
		}
		
		/**
		 Block method until a command is received from the ProcessAssitant (via pipe)
		 When a command is received, runCommand will be executed
		 runCommand can eventually call passCommand to sent messages back to the parent
		 */
		
		void run()
		{
		   // LOOP
			// Select
			// call runCommand(x)
			// Send "finish"
		   
		}
		
		/**
		 This command implementes how to run the process sent from the ProcessAssitant
		 This operation can be a map, a reduce, a generator, etc..
		 During this method execution, passCommand method can be called multiple time to pass commands to the user
		 Any of this commands can be neither finish nor error
		 
		 When runCommand finishes, a message is passed to the ProcessAssitant ( finish or error )
		 */
		
		void  runCommand( std::string command )
		{
		}
		
		
		/**
		 Method used to pass command back to the ProcessAssitant
		 */
		
		std::string passCommand(std::string command)
		{
		   // iomMsgSend();
		   // iomMsgAwait();
		   // iomMsgRead();
		}
		
		
	};

}


#endif

#ifndef _H_PROCESS_ASSISTANT_INTERFACE
#define _H_PROCESS_ASSISTANT_INTERFACE

#include <sstream>
#include <string>


namespace ss {


	class SamsonWorker;
	
	class ProcessAssistantInterface
	{
		
	public:
		
		int core;
		SamsonWorker* worker;
		std::string status;	// Internal state to be acess with getStatus
		
		ProcessAssistantInterface( int _core , SamsonWorker* _worker );		
		
		void setStatus( std::string txt )	// Internal function to set the status
		{
			std::ostringstream s;
			s << "Core " << core << " " << txt;
			status = s.str();
		}	

		// Main function to run everything
		virtual void run(void)=0;
		
		// get the status of this element
		virtual std::string getStatus()=0;

		
		// Get the i-th output queue name
		virtual std::string getOutputQueue( int i )
		{
			return "unasigned_queue_name";
		};

		virtual size_t getTaskId()
		{
			return 0;
		}
		
		void sendCloseMessages( size_t task_id, int workers );
		
		
		
		
	};	
	
}

#endif

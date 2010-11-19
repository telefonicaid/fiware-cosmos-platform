#ifndef _H_PROCESS_ASSISTANT_INTERFACE
#define _H_PROCESS_ASSISTANT_INTERFACE

#include <sstream>
#include <string>
#include "samson.pb.h"			// network::...

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
		std::string getStatus()
		{
			return status;
		}
		
		
		// Get the i-th output queue name
		virtual network::Queue getOutputQueue( int i )
		{
			assert( false );	
			// To avoid watining while this is not declared pure virtual
			network::Queue q;
			return q;
		};

		virtual size_t getTaskId()
		{
			return 0;
		}
		
		void sendCloseMessages( size_t task_id, int workers );
		
		
		
		
	};	
	
}

#endif

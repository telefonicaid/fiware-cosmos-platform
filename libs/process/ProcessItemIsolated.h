#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include "ProcessItem.h"		// ss::ProcessItem
#include "samson/Tracer.h"		// ss::Tracer


namespace ss {
	
	class ProcessItemIsolated : public ProcessItem , public Tracer
	{
		typedef enum 
		{
			starting,
			running,
			finished,
			broken
		} state;
		
		// Pipes used between two process
		int pipeFdPair1[2];
		int pipeFdPair2[2];

		state s;
		
	public:
		
		void run();
		
	public:
		
		// Function to be implemented ( running on a different process )
		virtual void runIsolated() = 0;

		// Function executed at this process side when a code is sent from the background process
		virtual void runCode( int c ) = 0;

		// Function used inside runIsolated to send a code to the main process
		void sendCode( int c );
		
		// Function used indide the runIsaled to send a trace to the main process
		void trace( int channel , const char *trace );
		
		// Function executed before and after
		virtual void init(){};
		virtual void finish(){};
		
		
	public:		

		// Internal function to be executed by a separate process or thread
		void runBackgroundProcessRun();
	
	private:
		
		const char* stateName()
		{
			switch (s) {
				case starting:
					return "starting";
				case running:
					return "running";
					break;
				case finished:
					return "finished";
					break;
				case broken:
					return "broken";
					break;
			}
			
			return "unknown";
		}
		
	};

	
};

#endif
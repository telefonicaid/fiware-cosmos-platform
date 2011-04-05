#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include "ProcessItem.h"		// ss::ProcessItem
#include "samson/Tracer.h"		// ss::Tracer
#include "samson/OperationController.h"		// ss::Tracer

// CODES FROM ISOLATED TO PLATFORM 

#define PI_CODE_REPORT_PROGRESS		-2
#define PI_CODE_USER_ERROR			-3
#define PI_CODE_TRACE				-4
#define PI_CODE_BEGIN_BACKGROUND	-5
#define PI_CODE_END_BACKGROUND		-6

//  CODES FROM PLATFORM TO ISOLATED

#define PI_CODE_CONTINUE			-3
#define PI_CODE_KILL				-4


namespace ss {
	
    class SharedMemoryItem;
    
	class ProcessItemIsolated : public  ProcessItem, public Tracer , public OperationController
	{
		typedef enum 
		{
			starting,
			running,
			finished,
			broken,			// Something is broken in the background process
			waiting			// Waiting the background process to die
		} state;
		
		// Pipes used between two process
		int pipeFdPair1[2];
		int pipeFdPair2[2];

		state s;

    public:
        
        int shm_id;							// Shared memory area used in this operation
		SharedMemoryItem *item;				// Share memory item
        
	public:
		
		void run();

        static bool isolated_process_as_tread;
		
	public:
		
		
		// Constructor
		ProcessItemIsolated();
		
        // Destructor ( free the shared memory segment used internally )
        ~ProcessItemIsolated();
        
		// Function to be implemented ( running on a different process )
		virtual void runIsolated() = 0;

		// Function executed at this process side when a code is sent from the background process
		// The returned value is the code send to the isolated process back ( contunue or kill )
		virtual int runCode( int c ) = 0;

		// Function used inside runIsolated to send a code to the main process
		void sendCode( int c );
		
		// Function used indide the runIsaled to send a trace to the main process
		void trace(LogLineData *logData);

		// Function used inside the runIsolated to send progress to the main process
		void reportProgress( double p );
		
		// Set the error and finish the task
		void setUserError( std::string message ); 
		
		// Function to specify if we are ready to be executed or continued from a halt	
		bool isReady();
		
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
				case waiting:
					return "waiting";
					break;
			}
			
			return "unknown";
		}
		
	};

	
};

#endif
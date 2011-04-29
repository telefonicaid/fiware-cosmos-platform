#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include "engine/ProcessItem.h"                    // ss::ProcessItem
#include "samson/Tracer.h"                  // ss::Tracer
#include "samson/OperationController.h"		// ss::Tracer
#include "engine/ProcessItem.h"
#include "SharedMemoryItem.h"               // engine::SharedMemoryItem
#include "samson.pb.h"                      // ss::network


namespace ss {
	
    
    
	class ProcessItemIsolated : public  engine::ProcessItem, public Tracer , public OperationController
	{
		
		// Pipes used between two process
		int pipeFdPair1[2];
		int pipeFdPair2[2];

    public:
        
        int shm_id;							// Shared memory area used in this operation
        engine::SharedMemoryItem *item;		// Share memory item
        
	public:
		
		void run();

        static bool isolated_process_as_tread;
		
    private:
        
        void runExchangeMessages();
        
	public:
		
		
		// Constructor
		ProcessItemIsolated();
		
        // Destructor ( free the shared memory segment used internally )
        ~ProcessItemIsolated();
        
		// Function to be implemented ( running on a different process )
		virtual void runIsolated() = 0;

		// Function executed at this process side when a code is sent from the background process
		// The returned value is the code send to the isolated process back ( contunue or kill )
		virtual void runCode( int c ) = 0;

		// Function used inside runIsolated to send a code to the main process
		void sendCode( int c );
		
		// Function used indide the runIsaled to send a trace to the main process
		void trace(LogLineData *logData);

		// Function used inside the runIsolated to send progress to the main process
		void reportProgress( double p );
		
		// Set the error and finish the task
		void setUserError( std::string message ); 

		// Generic function to send messages from process to platform
        void sendMessageProcessPlatform( ss::network::MessageProcessPlatform *message );

		// Function to specify if we are ready to be executed or continued from a halt	
		bool isReady();
		
	public:		

		// Internal function to be executed by a separate process or thread
		void runBackgroundProcessRun();
		
	};

	
};

#endif
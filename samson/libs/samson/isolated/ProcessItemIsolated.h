#ifndef _H_PROCESS_ITEM_ISOLATED
#define _H_PROCESS_ITEM_ISOLATED

#include "engine/ProcessItem.h"             // samson::ProcessItem
#include "samson/module/Tracer.h"           // samson::Tracer
#include "samson/module/OperationController.h"		// samson::Tracer
#include "engine/ProcessItem.h"
#include "SharedMemoryItem.h"               // engine::SharedMemoryItem
#include "samson/common/samson.pb.h"        // samson::network


namespace samson {
	
  
  
	class ProcessItemIsolated : public  engine::ProcessItem, public Tracer , public OperationController
	{
    
		// Pipes used between two process
		int pipeFdPair1[2];
		int pipeFdPair2[2];
    
  protected:
    
    std::string processItemIsolated_description;
    
	public:

    static bool isolated_process_as_tread; // Static falg to execute background processes as threads ( not forks )
		
    // Virtual methods of engine::ProcessItem
		virtual void run();
		
		// Constructor
		ProcessItemIsolated( std::string _description );
		
    // Destructor ( free the shared memory segment used internally )
    ~ProcessItemIsolated();
    
		// Function to be implemented ( running on a different process )
    virtual void initProcessItemIsolated()=0;
		virtual void runIsolated() = 0;
    virtual void finishProcessItemIsolated()=0;
    
		// Function executed at this process side when a code is sent from the background process
		// The returned value is the code send to the isolated process back ( contunue or kill )
		virtual void runCode( int c ) = 0;
    
		// Function used inside runIsolated to send a code to the main process
		void sendCode( int c );
		
		// Function used indide the runIsaled to send a trace to the main process
		void trace(LogLineData *logData);
    
		// Function used inside the runIsolated to send progress to the main process
		void reportProgress( double p );
		void reportProgress( double p , std::string status );
		
		// Set the error and finish the task
		void setUserError( std::string message ); 
    
		// Generic function to send messages from process to platform
    void sendMessageProcessPlatform( samson::gpb::MessageProcessPlatform *message );
        
		// Internal function to be executed by a separate process or thread
		void runBackgroundProcessRun();
		
    // Process a message from the background process
    // Return true if it necessary to finish
    bool processProcessPlatformMessage( samson::gpb::MessageProcessPlatform * message );
    
  private:
    
    void runExchangeMessages();

	};
  
	
};

#endif
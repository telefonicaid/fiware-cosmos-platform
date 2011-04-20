#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <string>
#include <sstream>
#include "au/Error.h"                  // au::Error
#include "au/Stopper.h"                // au::Stopper
#include "au/Environment.h"            // au::Environment

#include "EngineNotification.h"


#define PI_PRIORITY_NORMAL_OPERATION	1
#define PI_PRIORITY_NORMAL_COMPACT		5		// Compact operation
#define PI_PRIORITY_BUFFER_PREWRITE		10		// Processing an output buffer to generate what is stored on disk


/**
 
 ProcessItem
 
	item inside the ProcessManager
 
	An item is a task to do without any IO operation in the middle.
	It only requires processing time
 */

namespace engine {

	
	class ProcessItem;
	class ProcessManagerDelegate;
    class ProcessManager;
	
	class ProcessItem  : public Object
	{

	public:
		
		typedef enum 
		{
			queued,
			running,
			halted
		} ProcessItemStatus;
		
		ProcessItemStatus  state;
		
        // Pointer to the process manager
        ProcessManager* processManager;

		// Thread used to run this in background
		pthread_t t;
		
	private:
		
		au::Stopper stopper;	// Stopper to block the main thread until output memory is available again
		
	protected:

		// Information about the status of this process
		std::string operation_name;			// Name of the operation
		double progress;					// Progress of the operation ( if internally reported somehow )
		std::string sub_status;				// Letter describing internal status
		
	public:
		
		au::Error error;                    // Error management
		
		int priority;                       // Priority level ( 0 = low priority ,  10 = high priority )
		
	public:

        // Environment variables
        au::Environment environment;
        
		// Constructor with or without a delegate
		
		ProcessItem( int _priority );
		
		// Status management
		std::string getStatus();
		
		// Assign the delegate begore constructor
		void setProcessManagerDelegate( );
		
		// Notify delegate if any
		void notifyFinishToDelegate();		
		
		// What to do when processor is available
		virtual void run()=0;

		// Function to create a new thread and run "run" in background
		void runInBackground();

		// Function to check if the process if ready to be executed ( usually after calling halt )
		virtual bool isReady(){ return true; };
		
	protected:
		
		void halt();			// command executed inside run() to stop the task until output memory are free again
		
	public:
		
		void unHalt();			// Method to unhalt the process ( executed from the ProcessManager when output memory is available )

		
	};
	

}

#endif

#ifndef _H_PROCESS_ITEM
#define _H_PROCESS_ITEM


#include <string>
#include <sstream>
#include "Error.h"				// au::Error
#include "Stopper.h"			// au::Stopper

#define PI_PRIORITY_NORMAL_OPERATION	1

#define PI_PRIORITY_NORMAL_COMPACT		5		// Compact operation

#define PI_PRIORITY_BUFFER_PREWRITE		10		// Processing an output buffer to generate what is stored on disk


/**
 
 ProcessItem
 
	item inside the ProcessManager
 
	An item is a task to do without any IO operation in the middle.
	It only requires processing time
 */

namespace ss {

	
	class ProcessItem;
	class ProcessManager;
	
	/**
	 Delegate interface to receive notifications about finished ProcessItems
	 */
	
	class ProcessManagerDelegate
	{
		
	public:
		virtual void notifyFinishProcess( ProcessItem * item ) = 0;
		
	};
	
	
	class ProcessItem  
	{

	public:
		
		typedef enum 
		{
			queued,
			running,
			halted
		} ProcessItemStatus;
		
		ProcessItemStatus  state;
		
		// Pointer to the process manager to notify that we are finished
		ProcessManager *processManager;

		// Thread used to run this in background
		pthread_t t;
		
	private:
		
		au::Stopper stopper;	// Stopper to block the main thread until output memory is available again
		
	private:
		
		// Delegate to notify when finishing
		ProcessManagerDelegate * delegate;

		
	protected:

		// Information about the status of this process
		std::string operation_name;			// Name of the operation
		double progress;					// Progress of the operation ( if internally reported somehow )
		std::string sub_status;				// Letter describing internal status
		
	public:
		
		au::Error error;		// Error management
		
		int priority;			// Priority level ( 0 = low priority ,  10 = high priority )
		
	public:

		size_t component;		// Used by clients to indentify the component inside delegate to receive notiffication
		size_t tag;				// Used by clients to identify element inside delegate
		size_t sub_tag;			// Used by clients to identify sub-element inside delegate

		// Constructor with or without a delegate
		
		ProcessItem( int _priority );
		
		// Status management
		std::string getStatus();
		
		// Assign the delegate begore constructor
		void setProcessManagerDelegate( ProcessManagerDelegate * _delegate );
		
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

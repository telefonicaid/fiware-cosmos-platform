

#include "ProcessItem.h"		// ss::ProcessItem
#include "ProcessManager.h"		// ss::ProcessManager

namespace ss
{

#pragma mark Background call
	
	void* runProcessItem( void* p )
	{
		
		ProcessItem* processItem = (ProcessItem*) p;
		
		// Make sure we have the pointer to notify the process manager
		assert(processItem->processManager);
		
		// Run the process
		processItem->run();
		
		// Notify the process manager that we have finished
		processItem->processManager->notifyFinishProcessItem( processItem );
		
		// Notify the delegate about this finish
		processItem->notifyFinishToDelegate();
		
		
		return NULL;
	}
	
#pragma mark ----

	ProcessItem::ProcessItem(  int _priority )
	{

		priority = _priority;
		
		delegate = NULL;	
		
		operation_name = "unknown";	// Default message for the status
		
		progress = 0;		// Initial progress to "0"
		
		processManager = NULL;	// No init of the processManager pointer
		
		state = queued;
	}
	
	
	std::string ProcessItem::getStatus()
	{
		int p = progress*100.0;
		std::ostringstream o;
		
		switch (state) {
			case queued:
				o << "Q";
				break;
			case running:
				o << "R";
				break;
			case halted:
				o << "H";
				break;
		}
		
		o << ":" << priority;
		if ( sub_status.length() > 0)
			o << "," << sub_status;
		
		o << std::string(":") << operation_name;
		if ( (p> 0) && (p < 100))
			o << "(" << p << "%)";
		return o.str();
		
	}
	
	
	void ProcessItem::setProcessManagerDelegate( ProcessManagerDelegate * _delegate )
	{
		delegate = _delegate;
	}
	
	void ProcessItem::notifyFinishToDelegate()
	{
		if( delegate )
			delegate->notifyFinishProcess(this);
		else
		{
			// Autodelete if there is no delegate to handle notification
			delete this;
		}
	}
	
	void ProcessItem::runInBackground()
	{
		assert(processManager);
	
		// Create the thread as joinable to make sure we control when threads finish
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		
		pthread_create(&t, &attr, runProcessItem, this);
		pthread_attr_destroy( &attr );

	}
	
	void ProcessItem::halt()
	{
		state = halted;
		
		// Notify the ProcessManager about this
		
		processManager->notifyHaltProcessItem(this);
		
		// Stop this thread in the stopper loop
		stopper.stop();
		
		state = running;
	}
	
	void ProcessItem::unHalt()
	{
		// Wake up this process
		stopper.wakeUp();
	}
	
	
	
}
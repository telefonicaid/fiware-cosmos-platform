

#include "ProcessItem.h"		// ss::ProcessItem
#include "ProcessManager.h"		// ss::ProcessManager

namespace ss
{

	ProcessItem::ProcessItem(  ProcessManagerItemType _type )
	{
		type = _type;
		
		delegate = NULL;	
		
		status = "unknown";	// Default message for the status
		
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
		
		o << std::string(":") << status;
		if ( (p> 0) && (p < 100))
			o << "(" << p << "%)";
		return o.str();
		
	}

	void ProcessItem::setStatus(std::string _status)
	{
		status =  _status;
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
	
	
	void* runProcessItem( void* p )
	{
		
		ProcessItem* processItem = (ProcessItem*)p;
		
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
	
	void ProcessItem::runInBackground()
	{
		assert(processManager);
		
		pthread_t t;
		pthread_create(&t, NULL, runProcessItem, this);
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
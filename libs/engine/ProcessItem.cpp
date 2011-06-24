

#include "engine/ProcessItem.h"				// engine::ProcessItem
#include "engine/Engine.h"					// engine::Engine
#include "engine/ProcessManager.h"          // engine::ProcessManager

namespace engine
{
	
	
#pragma mark Background call
	
	void* runProcessItem( void* p )
	{
		
		ProcessItem* processItem = (ProcessItem*) p;
		
		// Run the process
		processItem->run();
		
		// Notify the process manager that we have finished
		processItem->processManager->finishProcessItem( processItem );
		
		return NULL;
	}
	
#pragma mark ----

	ProcessItem::ProcessItem(  int _priority )
	{

		priority = _priority;
		operation_name = "unknown";	// Default message for the status
		progress = 0;		// Initial progress to "0"
		state = queued;
        
        canceled = false;   // By default it is not canceled
        
        listenerId = 0; // By default, nobody is notified when finished
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

		o << std::string(":") << operation_name;
        
		if ( sub_status.length() > 0)
			o << ":" << sub_status ;
		
		if ( (p> 0) && (p < 100))
			o << " (" << p << "%)";
		return o.str();
		
	}
	

	
	void ProcessItem::runInBackground()
	{
		// Create the thread as joinable to make sure we control when threads finish
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		
		pthread_create(&t, &attr, runProcessItem, this);
		pthread_attr_destroy( &attr );

	}
	
	void ProcessItem::halt()
	{
		stopper.stop_begin();
        
		state = halted;
        
		// Notify the ProcessManager about this
		processManager->haltProcessItem(this);
		
		// Stop this thread in the stopper loop
		stopper.stop_finish( 0 );

		// Change the state variable to running
        stopper.lock();

        // come back to the running state
        state = running;
        
        stopper.unlock();
        
	}
	
	void ProcessItem::unHalt()
	{
		// Wake up this process
		stopper.wakeUp();
	}
    
    void ProcessItem::setCanceled()
    {
        canceled = true;    
    }
    
    bool ProcessItem::isProcessItemCanceled()
    {
        return canceled;
    }
    
    void ProcessItem::setListenerId( size_t _listenerId )
    {
        listenerId = _listenerId;
    }
    
    
	
}
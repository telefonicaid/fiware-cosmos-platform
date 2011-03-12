

#include "ProcessManager.h"				// Own interface
#include "ProcessItem.h"				// ss::ProcessItem
#include "SamsonSetup.h"				// ss::SamsonSetup
#include <sstream>						// std::ostringstream
#include "MemoryManager.h"				// ss::MemoryManager

namespace ss
{
	
	static ProcessManager *processManager;

	void *run_thread_process_manager(void*p)
	{
		ProcessManager* tmp = (ProcessManager*) p;
		tmp->runThread();
		return NULL;
	}
	
	void ProcessManager::init()
	{
		assert( ! processManager );// Init only once
		processManager = new ProcessManager();	  
	}
	
	ProcessManager* ProcessManager::shared()
	{
		assert( processManager );
		return processManager;
	}
	
	ProcessManager::ProcessManager ( )
	{
		// Take the num of process from setup and init the current number of concurrent process
		num_processes = SamsonSetup::shared()->num_processes;
		
		// Run main thread
		pthread_t t;
		pthread_create(&t, NULL, run_thread_process_manager, this);
		
	}
	
	void ProcessManager::addProcessItem( ProcessItem *item )
	{
		token.retain();
		
		switch (item->type) {
			case ProcessItem::pure_process:
				items_pure_process.push_back( item );		// Insert in the list
				break;
			case ProcessItem::data_generator:
				items_data_generator.push_back( item );		// Insert in the list
				break;
		}

		token.release();
		
		stopper.wakeUp();				// Wake up main thread to see if it is necessary to runa  new thread
		
	}
	
	/**
	 Function executed by all threads running things
	 */
	
	void ProcessManager::runThread()
	{
		while(true)
		{
			
			// Get the next process item to process ( if CPU slots available )
			ProcessItem * item = NULL;
			bool halted_process = false;

			// Get the next item to process ( protect with a mutex )
			token.retain();
			
			
			if( (int)running_items.size() < num_processes )
			{
				// Priority for the pure processes	
				item = items_pure_process.extractFront();
				
				if( !item )
				{

					if( halted_items.size() > 0 )
					{
						// Check if there is output memory prior to continue this execution
						if( MemoryManager::shared()->availableMemoryOutput() )
							item = halted_items.extractFront();
						
					}
					else
						item = items_data_generator.extractFront();
				}
			}

			if( item )
			{
				item->processManager =  this;	// Set a pointer to me to be notified
				running_items.insert( item );
			}
			
			halted_process = (halted_items.size() > 0 );
			
			token.release();

			// Run this item in a separate thread or block until next event
			if( item )
			{
				switch (item->state) {
					case ProcessItem::queued:
						item->state = ProcessItem::running;
						item->runInBackground();
						break;
					case ProcessItem::halted:
						item->state = ProcessItem::running;
						item->unHalt();
						break;
					default:
						assert(false);
						break;
				}
			}
			else
			{
				if( halted_process )
					stopper.stop(1);
				else
					stopper.stop();
			}
			
		}
	}
	
	
	void ProcessManager::notifyFinishProcessItem( ProcessItem *item )
	{
		token.retain();
		running_items.erase(item);
		token.release();
		
		// Wake up the main thread to start new threads if necessary
		stopper.wakeUp();
		
	}
	
	void ProcessManager::notifyHaltProcessItem( ProcessItem *item )
	{
		token.retain();

		running_items.erase(item);
		halted_items.push_back(item);
		
		token.release();
		
		// Wake up the main thread to start new threads if necessary
		stopper.wakeUp();
		
	}
	
	std::string ProcessManager::getStatus()
	{
		std::ostringstream output;
		
		token.retain();
		output << "\n\t\t\tRunning: ";
		for ( std::set<ProcessItem*>::iterator i = running_items.begin () ; i != running_items.end() ; i++ )
			output << "[" << (*i)->getStatus() << "] ";

		output << "\n\t\t\tHalted: ";
		for ( std::list<ProcessItem*>::iterator i = halted_items.begin () ; i != halted_items.end() ; i++ )
			output << "[" << (*i)->getStatus() << "] ";

		output << "\n\t\t\tQueued: " << items_pure_process.size() << " & " << items_data_generator.size();
		
		/*
		output << "\n\t\t\tQueued: ";
		for ( std::list<ProcessItem*>::iterator i = items_pure_process.begin () ; i != items_pure_process.end() ; i++ )
			output << "[" << (*i)->getStatus() << "] ";
		output << "\n\t\t\tQueued: ";
		for ( std::list<ProcessItem*>::iterator i = items_data_generator.begin () ; i != items_data_generator.end() ; i++ )
			output << "[" << (*i)->getStatus() << "] ";
		*/
		
		token.release();
		
		return output.str();
	}
	
	void ProcessManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_process_manager_status( getStatus() );
	}	
}

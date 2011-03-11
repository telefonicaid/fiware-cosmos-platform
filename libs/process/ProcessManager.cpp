

#include "ProcessManager.h"				// Own interface
#include "ProcessItem.h"				// ss::ProcessItem
#include "SamsonSetup.h"				// ss::SamsonSetup
#include <sstream>				// std::ostringstream

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
		num_current_processes = 0;
		
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
			
			// Stop until there are less current process than allowed
			while( num_current_processes >= num_processes )
				stopper.stop();
			
			// Get the next process item to process
			ProcessItem * item = NULL;
			while( !item )
			{
				// Get the next item to process ( protect with a mutex )
				token.retain();
				
				item = items_pure_process.extractFront();
				if( !item )
					item = items_data_generator.extractFront();
				
				token.release();

				if( !item )
					stopper.stop();	// Stop this thread
			}

			// Create a thread to run this process
			token.retain();
			item->processManager =  this;	// Set a pointer to me to be notified
			running_items.insert( item );
			num_current_processes++;
			token.release();

			// Run this item in a separate thread
			item->runInBackground();
			
		}
	}
	
	
	void ProcessManager::notifyFinishProcessItem( ProcessItem *item )
	{
		token.retain();
		
		running_items.erase(item);
		num_current_processes--;
		
		token.release();
		
		stopper.wakeUp();
		
	}
	
	
#pragma mark DEBUG
	
	
	std::string ProcessManager::getStatus()
	{
		std::ostringstream output;
		
		token.retain();
		
		for ( std::set<ProcessItem*>::iterator i = running_items.begin () ; i != running_items.end() ; i++ )
			output << "[" << (*i)->getStatus() << "] ";
		
		output << " ( " << items_pure_process.size()+items_data_generator.size() << " queued processes )";
		
		token.release();
		
		return output.str();
	}
	
	void ProcessManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_process_manager_status( getStatus() );
	}	
}

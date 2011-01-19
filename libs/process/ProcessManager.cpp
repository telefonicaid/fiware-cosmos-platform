

#include "ProcessManager.h"				// Own interface
#include "ProcessItem.h"				// ss::ProcessItem
#include "SamsonSetup.h"				// ss::SamsonSetup
#include <sstream>				// std::ostringstream

namespace ss
{
	
	static ProcessManager *processManager;
	
	class ProcessManagerRunInfo
	{
	public:
		
		int id;
		ProcessManager* processManager;
	};
	
	void *run_thread_process_manager(void*p)
	{
		ProcessManagerRunInfo* tmp = (ProcessManagerRunInfo*) p;

		tmp->processManager->runThread( tmp->id );
		
		delete tmp;
		
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
		// Take the num of process from setup
		num_processes = SamsonSetup::shared()->num_processes;
		
		// Id counter to assign a new id to each process
		id = 0;
		
		// Pointers to the running items
		runing_item = (ProcessItem**) malloc( sizeof( ProcessItem * ) * num_processes );
		
		// Runt he threads
		for (int i = 0 ; i < num_processes ; i++)
		{
			runing_item[i] = NULL;
			pthread_t t;
			
			ProcessManagerRunInfo *tmp = new ProcessManagerRunInfo();
			tmp->id = i;
			tmp->processManager = this;
			
			pthread_create(&t, NULL, run_thread_process_manager, tmp);
		}
		
	}
	
	void ProcessManager::addProcessItem( ProcessItem *item )
	{
		token.retain();
		
		// Give an id to each ProcessItem
		item->id_processItem =  id++;
		
		items.push_back( item );
		
		token.release();
		
		stopper.wakeUp();
		
	}
	
	/**
	 Function executed by all threads running things
	 */
	
	void ProcessManager::runThread(int id)
	{
		while(true)
		{
			// Get the next element to process
			ProcessItem *item = getNextItemToProcess();
			
			runing_item[id] = item;	// For debuggin
			
			// Execute
			item->run();

			runing_item[id] = NULL;	// For debuggin
			
			// Notify about the end of the execution
			item->notifyFinishToDelegate();
		}
	}
	
	/**
	 Get the next item to process
	 */
	
	ProcessItem * ProcessManager::getNextItemToProcess()
	{
		
		while( true )
		{
			// Get the next item to process ( protect with a mutex )
			token.retain();
			ProcessItem * item = items.extractFront();
			token.release();
			
			if( item )
				return item;
			
			stopper.stop();
		}
		
	}
	
	
#pragma mark DEBUG
	
	
	std::string ProcessManager::getStatus()
	{
		std::ostringstream output;
		
		token.retain();
		
		for (int i = 0 ; i < num_processes ; i++)
		{
			output << "[";
			if( runing_item[i] )
				output << runing_item[i]->getStatus();
			else
				output << "Idle";
			output << "] ";
		}
		token.release();
		
		return output.str();
	}
	
	void ProcessManager::fill(network::WorkerStatus*  ws)
	{
		ws->set_process_manager_status( getStatus() );
	}	
}

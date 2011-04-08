

#ifndef _H_SAMSON_PROCESS_MANAGER
#define _H_SAMSON_PROCESS_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include "au_map.h"                 // au::list
#include "MemoryManager.h"          // ss::MemoryManager
#include "SharedMemoryManager.h"    // ss::SharedMemoryManager
#include "DiskStatistics.h"         // ss::DiskStatistics
#include <set>                      // std::set
#include "EngineNotification.h"     // ss::EngineNotification
#include "au_map.h"                 // au::set

namespace ss
{
    
	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class EngineNotification;
    
    /**
     Class to manage background jobs
     */
    
    class ProcessManager : public EngineNotificationListener
    {
        
		// Background processes
		// --------------------------------------------------------------------
		pthread_mutex_t mutex;                      // Mutex to protect the background process notifications
		int num_processes;							// Number of maximum simultaneous process running ( from setup )
		au::set<ProcessItem> items;				// List of items to be executed ( all priorities  )
		au::set<ProcessItem> running_items;		// Set of items currently being executed
		au::set<ProcessItem> halted_items;		// Set of items currently being executed but halted
        
    public:
        
        ProcessManager();
        ~ProcessManager();
        
        void notify( EngineNotification* notification );
        
		void addProcessItem( ProcessItem *item );			// Function to add a Process. It will be notifier by delegate mechanism
		void finishProcessItem( ProcessItem *item );		// Notification that this ProcessItem has finished
		void haltProcessItem( ProcessItem *item );			// Notification that this ProcessItem is halted ( blocked until output memory is ready ) 
		void checkBackgroundProcesses();	// Check background process in order to see if new threads have to be created
        
		// Function to fill part of the message sent to the controller ( informing about status )
		void fill(network::WorkerStatus*  ws);				
        
        // Remove pending stuff and wait for the running
        void quit();
        
    private:
        
        // Get the next background element to be executed in a parallel thread
		ProcessItem* _getNextItemToRun();
        
        
    };

}

#endif
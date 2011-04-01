

#ifndef _H_SAMSON_DISK_MANAGER
#define _H_SAMSON_DISK_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include "au_map.h"                 // au::list
#include "MemoryManager.h"          // ss::MemoryManager
#include "DiskStatistics.h"         // ss::DiskStatistics
#include <set>                      // std::set
#include "EngineNotification.h"     // ss::EngineNotification

namespace ss
{
    
	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class EngineNotification;
    
    
    class DiskManager : public EngineNotificationListener
    {
        
		// Disk Operations
		pthread_mutex_t mutex;                          // Mutex to protect the background threads performing operations
		int num_disk_operations;						// Number of paralell disk operations allowed
		au::list<DiskOperation> pending_operations;		// List of pending operations
		std::set<DiskOperation*> running_operations;	// Running operations
		DiskStatistics diskStatistics;                  // Statistics
        
    public:
        
        DiskManager();
        ~DiskManager();
        
        void notify( EngineNotification* notification );
        
		void add( DiskOperation *operation );				// Add a disk operation to be executed in the background
		void finishDiskOperation( DiskOperation *diskOperation );	// Notification that a disk operation has finished
		void checkDiskOperations();			// Check if we can run more disk operations
        
		// Function to fill part of the message sent to the controller ( informing about status )
		void fill(network::WorkerStatus*  ws);				
        
    };

}

#endif
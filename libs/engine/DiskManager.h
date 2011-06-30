

#ifndef _H_SAMSON_DISK_MANAGER
#define _H_SAMSON_DISK_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <string.h>
#include <iostream>                 // std::cout
#include <set>                      // std::set

#include "au/list.h"                 // au::list
#include "au/ListMap.h"              // au::ListMap

#include "engine/Object.h"     // engine::EngineNotification
#include "engine/MemoryManager.h"          // engine::MemoryManager
#include "engine/DiskStatistics.h"         // engine::DiskStatistics

#include "engine/ReadFileManager.h"        // engine::ReadFileManager

#define notification_disk_operation_request_response    "notification_disk_operation_request_response"

namespace engine
{
    
	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class Notification;
    
    class DiskManager 
    {
        
        friend class DiskOperation;
        
        // File manager ( conatins all the open files )
        ReadFileManager fileManager;
        
		// Disk Operations
		pthread_mutex_t mutex;                          // Mutex to protect the background threads performing operations
		int num_disk_operations;						// Number of paralell disk operations allowed
        
		au::list<DiskOperation> pending_operations;		// List of pending operations
		std::set<DiskOperation*> running_operations;	// Running operations
        
		DiskStatistics diskStatistics;                  // Statistics

        DiskManager( int _num_disk_operations );
        
    public:

        static void init( int _num_disk_operations );
        static DiskManager* shared();
        
        ~DiskManager();

		void add( DiskOperation *operation );				// Add a disk operation to be executed in the background
		void cancel( DiskOperation *operation );			// Add a disk operation to be executed in the background
        
    private:
        
		void checkDiskOperations();                         // Check if we can run more disk operations
        
        // Remove pending operations and wait for the running ones
        void quit();
        

    public:
        
        int getNumOperations();                  // get the number of pending operations ( shown at information screen);
        size_t getReadRate();                  
        size_t getWriteRate();                 
        std::string str();
        
    private:
        
		void finishDiskOperation( DiskOperation *diskOperation );	// Notification that a disk operation has finished        
        
        au::list<DiskOperation>::iterator _find_pos( DiskOperation *diskOperation );
        
        
    };

}

#endif

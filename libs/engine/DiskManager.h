

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

#include "engine/EngineNotification.h"     // engine::EngineNotification
#include "engine/MemoryManager.h"          // engine::MemoryManager
#include "engine/DiskStatistics.h"         // engine::DiskStatistics

#include "engine/ReadFileManager.h"        // engine::ReadFileManager

#define notification_disk_operation_request             "notification_disk_operation_request"
#define notification_disk_operation_request_response    "notification_disk_operation_request_response"

namespace engine
{
    
	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class Notification;
    
  
    
    
    class DiskManager : public NotificationListener
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
        
        std::string _str();
        
    public:

        static void init( int _num_disk_operations );
        static std::string str();
        static int getNumOperations();                  // get the number of pending operations ( shown at information screen);
        
        ~DiskManager();

        // Receive notifications from the Engine
        void notify( Notification* notification );
        
		void add( DiskOperation *operation );				// Add a disk operation to be executed in the background
		void checkDiskOperations();			// Check if we can run more disk operations
        
        // Remove pending operations and wait for the running ones
        void quit();
        

    private:
        
		void finishDiskOperation( DiskOperation *diskOperation );	// Notification that a disk operation has finished        
        
        int _getNumOperations();                  // get the number of pending operations ( shown at information screen);
        
    };

}

#endif

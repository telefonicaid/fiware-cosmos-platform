

#ifndef _H_SAMSON_PROCESS_MANAGER
#define _H_SAMSON_PROCESS_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include "au/map.h"                 // au::list
#include "MemoryManager.h"          // ss::MemoryManager
#include "DiskStatistics.h"         // ss::DiskStatistics
#include <set>                      // std::set
#include "EngineNotification.h"     // ss::EngineNotification
#include "au/set.h"                 // au::set


#define notification_process_request                            "notification_process_request"
#define notification_process_request_response                   "notification_process_request_response"
#define notification_process_manager_check_background_process   "notification_process_manager_check_background_process"
#define notification_process_cancel                             "notification_process_cancel"

namespace engine
{
    
	class EngineElement;
	class ProcessItem;
	class DiskOperation;
	class Notification;
    
    /**
     Class to manage background jobs
     */
    
    class ProcessManager : public NotificationListener
    {
        
		// Background processes
		// --------------------------------------------------------------------
        
		pthread_mutex_t mutex;                  // Mutex to protect the background process notifications
		int num_processes;						// Number of maximum simultaneous process running ( from setup )
		au::set<ProcessItem> items;				// List of items to be executed ( all priorities  )
		au::set<ProcessItem> running_items;		// Set of items currently being executed
		au::set<ProcessItem> halted_items;		// Set of items currently being executed but halted
		au::set<ProcessItem> canceled_items;	// Set of items currently being executed but canceled ( so waiting to be finished )
        
        ProcessManager( int _num_processes);
        
        std::string _str();
        
    public:
        
        ~ProcessManager();
        
        static void init( int _num_processes);
        static std::string str();
        static int getNumCores();
        static int getNumUsedCores();

        // All the interface is using notification mechanism
        void notify( Notification* notification );

    public:

        // Publics but only called from SAMSON platform
		void finishProcessItem( ProcessItem *item );		// Notification that this ProcessItem has finished
		void haltProcessItem( ProcessItem *item );			// Notification that this ProcessItem is halted ( blocked until output memory is ready ) 
        
    private:

		void _addProcessItem( ProcessItem *item );			// Function to add a Process. It will be notifier by delegate mechanism
        
		void _checkBackgroundProcesses();                    // Check background process in order to see if new threads have to be created
                
        // Remove pending stuff and wait for the running
        void quit();
        
        // Get the next background element to be executed in a parallel thread
		ProcessItem* _getNextItemToRun();
        
        
    };

}

#endif

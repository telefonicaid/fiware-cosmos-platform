/* ****************************************************************************
 *
 * FILE            ProcessManager
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * Manager of background tasks to be executed by a set of cores.
 * It creates a separate thread for each task and controlls them.
 *
 * ****************************************************************************/

#ifndef _H_SAMSON_PROCESS_MANAGER
#define _H_SAMSON_PROCESS_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <iostream>                 // std::cout
#include <set>                      // std::set

#include "au/map.h"                 // au::list
#include "au/set.h"                 // au::set
#include "au/Token.h"               // au::Token
#include "au/namespace.h"

#include "engine/Engine.h"     // engine::Engine
#include "engine/Object.h"     // engine::EngineNotification
#include "engine/MemoryManager.h"          // engine::MemoryManager
#include "engine/DiskStatistics.h"         // engine::DiskStatistics
#include "engine/Object.h"                  // engine::Object

#define notification_process_request_response                   "notification_process_request_response"
#define notification_process_manager_check_background_process   "notification_process_manager_check_background_process"

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

/**
 Class to manage background jobs
 */

class ProcessManager  : public engine::Object , engine::EngineService
{
    
    // Mutex to protect differnt queues
    au::Token token;
    
    // Background processes
    // --------------------------------------------------------------------
    
    int num_processes;						// Number of maximum simultaneous process running ( from setup )
    au::set<ProcessItem> items;				// List of items to be executed ( all priorities  )
    au::set<ProcessItem> running_items;		// Set of items currently being executed
    au::set<ProcessItem> halted_items;		// Set of items currently being executed but halted
    
    ProcessManager( int _num_processes);
    
    //Statistics
    au::map<std::string,au::SimpleRateCollection> rates;    // Information about rates
    
public:
    
    ~ProcessManager();
    
    static void init( int _num_processes );
    static ProcessManager* shared();
    
    static int getNumCores();
    static int getNumUsedCores();
    
    void notify( Notification* notification );
    
public:
    
    // Publics but only called from SAMSON platform
    void finishProcessItem( ProcessItem *item );		// Notification that this ProcessItem has finished
    void haltProcessItem( ProcessItem *item );			// Notification that this ProcessItem is halted ( blocked until output memory is ready ) 
    
public:
    
    void add( ProcessItem *item , size_t listenerId );                      // Function to add a Process. It will be notifier by delegate mechanism
    void cancel( ProcessItem *item );                   // Function to cancel a Process. 
    
    bool hasFreeCores();    // Check if there are less processes than cores
    
public:
    
    // Get information for monitoring
    void getInfo( std::ostringstream& output);
    
private:
    
    void checkBackgroundProcesses();                    // Check background process in order to see if new threads have to be created
    
    // Remove pending stuff and wait for the running
    void quit();
    
    // Get the next background element to be executed in a parallel thread
    ProcessItem* _getNextItemToRun();
    
    
};

NAMESPACE_END

#endif

/* ****************************************************************************
 *
 * FILE            DiskManager
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * DiskManager is the entity in charge of schedulling all the read/write/remove acetions over file system
 *
 * ****************************************************************************/

#ifndef _H_SAMSON_DISK_MANAGER
#define _H_SAMSON_DISK_MANAGER

#include <pthread.h>
#include <list>
#include <string>
#include <string.h>
#include <iostream>                 // std::cout
#include <set>                      // std::set

#include "au/containers/list.h"                 // au::list
#include "au/mutex/Token.h"                 // au::Token
#include "au/containers/ListMap.h"              // au::ListMap
#include "au/namespace.h"
#include "au/Rate.h"
#include "au/OnOffMonitor.h"

#include "engine/Object.h"     // engine::EngineNotification
#include "engine/MemoryManager.h"          // engine::MemoryManager
#include "engine/Engine.h"

#include "engine/ReadFileManager.h"        // engine::ReadFileManager

#define notification_disk_operation_request_response    "notification_disk_operation_request_response"

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;


class DiskManager
{
    //singleton instance
    static DiskManager *diskManager;

    friend class DiskOperation;
    
    // File manager ( containing all the open files )
    ReadFileManager fileManager;
    
    // Disk Operations
    au::Token token;

    bool quitting;                                  // Flag to indicate background processes to quit
    int num_disk_operations;						// Number of paralell disk operations allowed
    int num_disk_manager_workers;                   // Number of parallel workers for Disk operations
    
    au::list<DiskOperation> pending_operations;		// List of pending operations
    std::set<DiskOperation*> running_operations;	// Running operations
    
    DiskManager( int _num_disk_operations );
    
public:

    au::rate::Rate rate_in;
    au::rate::Rate rate_out;
    
    au::OnOffMonitor on_off_monitor;
    
    static void init( int _num_disk_operations );
    static void stop( );
    static void destroy( );
    static DiskManager* shared();
   
    ~DiskManager();
    
    void add( DiskOperation *operation );				// Add a disk operation to be executed in the background
    void cancel( DiskOperation *operation );			// Add a disk operation to be executed in the background
    
public:

    // Main function for the background worker
    // It is public only to be called form the thread-creation function
    void run_worker();  
    
public:
    
    int getNumOperations();                  // get the number of pending operations ( shown at information screen);
    
    //std::string str();
    void getInfo( std::ostringstream& output);
    
    void setNumOperations( int _num_disk_operations );
    
    size_t get_rate_in();
    size_t get_rate_out();
    double get_rate_operations_in();
    double get_rate_operations_out();
    double get_on_off_activity();

private:
    
    // Notification that a disk operation has finished
    void finishDiskOperation( DiskOperation *diskOperation );	        
    
    // Auxiliar function usedto insert new disk operations in place
    au::list<DiskOperation>::iterator _find_pos( DiskOperation *diskOperation );

    // Auxiliar function to get the next operation ( NULL if no more disk operations )
    DiskOperation * getNextDiskOperation();
    
    void add_worker( );
    bool check_quit_worker(  );
    
    int get_num_disk_manager_workers();
    void createThreads();
    
};

NAMESPACE_END

#endif

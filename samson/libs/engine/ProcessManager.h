/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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

#include "au/containers/map.h"                 // au::list
#include "au/containers/set.h"                 // au::set
#include "au/mutex/Token.h"               // au::Token
#include "au/namespace.h"

#include "engine/Engine.h"     // engine::Engine
#include "engine/Object.h"     // engine::EngineNotification
#include "engine/MemoryManager.h"          // engine::MemoryManager
#include "engine/Object.h"                  // engine::Object

#define notification_process_request_response                   "notification_process_request_response"

NAMESPACE_BEGIN(engine)

class EngineElement;
class ProcessItem;
class DiskOperation;
class Notification;

/**
 Class to manage background jobs
 */

class ProcessManager  :  engine::Object
{
    static ProcessManager* processManager;  //Singleton Instance pointer
    
    // Mutex to protect different queues
    au::Token token;
    
    au::set<ProcessItem> items;				// List of items to be executed ( all priorities  )
    au::set<ProcessItem> running_items;		// Set of items currently being executed

    int num_processes;						// Number of maximum simultaneous process running ( from setup )

    // Private constructor to be a singleton
    ProcessManager( int _num_processes);
    
    pthread_t t_scheduler; // Thread continuously checking for new process to be executed

public:
    
    bool thread_running;   // Flag to indicate that the background thread is running
    bool quitting;         // Flag to indicate that we are quitting
    
public:
    
    int public_max_proccesses;
    int public_num_proccesses;
    
public:
    
    ~ProcessManager();
    
    static void init( int _num_processes );
    static void stop( );
    static void destroy( );
    static ProcessManager* shared();
    
    static int getNumCores();
    static int getNumUsedCores();
    
    void notify( Notification* notification );
    
public:
    
    // Function to add a Process. It will be notifier by delegate mechanism
    void add( ProcessItem *item , size_t listenerId );                          
    
    // Function to cancel a Process. 
    void cancel( ProcessItem *item );                   
    
public:
    
    // Notification that this ProcessItem has finished
    // Public but only called from SAMSON platform ( background process notifying ... )
    void finishProcessItem( ProcessItem *item );		
    
public:
    
    // Get information for monitoring
    void getInfo( std::ostringstream& output);
    
public:
    
    // Only public to be executed from a separate thread
    // Check background process in order to see if new threads have to be created
    void check_background_processes();                    
    
    // Run next item if possible
    void run_next_items();
    
private:
 
    // Operations over the lists of processes with token protection
    
    void token_add( ProcessItem* item );
    ProcessItem* token_getNextProcessItem();
    ProcessItem* token_finishProcessItem( ProcessItem* item );
    ProcessItem* token_cancelProcessItem( ProcessItem* item );
    void token_getInfo( std::ostringstream& output);
    size_t token_getNumRunningProcessItem();
    
};

NAMESPACE_END

#endif

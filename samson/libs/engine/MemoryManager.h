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
 * FILE            MemoryManager
 *
 * AUTHOR          Andreu Urruela
 *
 * DATE            July 2011
 *
 * DESCRIPTION
 *
 * MemoryManager is the controller of the "big" blocks of memory used by the app
 * Buffers can be allocated directly using newBuffer.
 * Buffers can be allocated with add( MemoryRequest* ) to be served when memory is available
 * A Notification ( using engine mechanism ) is sent when memory is available
 *
 * ****************************************************************************/


#ifndef _H_MEMORY_MANAGER
#define _H_MEMORY_MANAGER

#include <stdio.h>              // perror
#include <sys/types.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <map>						// std::map
#include <set>						// std::set
#include <list>						// std::list
#include <iostream>					// std::cout

#include "engine/Buffer.h"                  // samson::Buffer
#include "engine/Engine.h"                  // samson::Buffer
#include "au/mutex/Token.h"                       // au::Token
#include "au/containers/map.h"                         // au::map
#include "au/containers/list.h"                        // au::list
#include "au/string.h"                      // au::Format
#include "au/namespace.h"

#include "au/tables/Table.h"

#define notification_memory_request_response    "notification_memory_request_response"

NAMESPACE_BEGIN(engine)

class MemoryRequest;

/**
 
 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
 It is also responsible to manage shared-memory areas to share between differnt processes. 
 
 */

class MemoryManager
{
    static MemoryManager* memoryManager;            // Singleton Instance pointer
    au::Token token;                                // Token to protect this instance and memoryRequests
                                                    // It is necessary to protect since network thread can access directly here
    
    size_t memory;                                  // Total available memory
    
    // List of memory requests
    au::list <MemoryRequest> memoryRequests;        // Request for memory
    
    // List of active buffers for better monitoring
    std::set<Buffer*> buffers;
    
    // Private constructor for
    MemoryManager( size_t _memory );
    
    friend class Buffer;            // Buffer class can call to destoryBuffer
    friend class BufferContainer;   // Buffer container is the only one that can create Buffers directly
    
public:
    
    size_t public_max_memory;
    size_t public_used_memory;
    
public:
    
    static void destroy( );
    static void init( size_t _memory );
    static MemoryManager *shared();
    
    ~MemoryManager();
    
    /*
     --------------------------------------------------------------------
     DIRECT mecanish to request buffers ( synchronous interface )
     --------------------------------------------------------------------
     */

    Buffer *createBuffer( std::string name , std::string type ,  size_t size , double mem_limit=0.0 );

private:
    
    Buffer *_newBuffer( std::string name , std::string type , size_t size , double mem_limit );
    
    
    /*
     --------------------------------------------------------------------
     INDIRECT mecanish to request buffers ( asynchronous interface )
     --------------------------------------------------------------------
     */
    
    
public:
    
    void add( MemoryRequest *request );
    
    void cancel( MemoryRequest *request );
    
private:
    
    void _checkMemoryRequests();         // Check the pending memory requests
    
    
private:

    // This method is called automatically when relese a Buffer that nodoby retained
    void destroyBuffer( Buffer *b );    		 //Interface to destroy a buffer of memory
    
public:
    
    /*
     --------------------------------------------------------------------
     Get information about memory usage
     --------------------------------------------------------------------
     */
    
    size_t getMemory();
    
    int getNumBuffers();
    size_t getUsedMemory();
    double getMemoryUsage();
    
    
    
private:
    
    double _getMemoryUsage();
    size_t _getUsedMemory();
    
public:
    
    au::tables::Table getTableOfBuffers();    
};

NAMESPACE_END

#endif

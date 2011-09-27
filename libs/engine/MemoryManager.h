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
#include "au/Token.h"                       // au::Token
#include "au/map.h"                         // au::map
#include "au/list.h"                        // au::list
#include "au/string.h"                      // au::Format


#define notification_memory_request_response    "notification_memory_request_response"

namespace engine {

    class MemoryRequest;
    
	/**
	 
	 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
	 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
	 It is also responsible to manage shared-memory areas to share between differnt processes. 
	 
	 */
	
	class MemoryManager 
	{
		
		au::Token token;							// Token to protect this instance and memoryRequests
													// It is necessary to protect since network thread can access directly here

		size_t memory;								// Total available memory
				
		// List of memory requests
		au::list <MemoryRequest> memoryRequests;        // Only used for inputs ( tag == 0)
		
        // List of active buffers for better monitorization
        std::set<Buffer*> buffers;

        // Private constructor for
		MemoryManager( size_t _memory );
		
	public:

        static void init( size_t _memory );
        static void destroy( );

        static MemoryManager *shared();
        
		~MemoryManager();


		 /*
         --------------------------------------------------------------------
		 DIRECT mecanish to request buffers ( synchronous interface )
		 --------------------------------------------------------------------
          */
		
		Buffer *newBuffer( std::string name ,  size_t size , int tag );

	private:
		
		Buffer *_newBuffer( std::string name ,  size_t size , int tag );

        
        /*
         --------------------------------------------------------------------
		 INDIRECT mecanish to request buffers ( asynchronous interface )
		 --------------------------------------------------------------------
        */
        
        
    public:
        
        void add( MemoryRequest *request );

        void cancel( MemoryRequest *request );
        
    private:
        
        void checkMemoryRequests();         // Check the pending memory requests

        
        /*
         --------------------------------------------------------------------
		 DIRECT AND INDIRECT mecanish to destroy a buffer 
         ( synchronous & asynchronous interface )
		 --------------------------------------------------------------------
         */
        
	public:
		
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
        
        
		int getNumBuffersByTag( int tag );
        size_t getUsedMemoryByTag( int tag );
        double getMemoryUsageByTag( int tag );

    private:
		
        double _getMemoryUsage();
        size_t _getUsedMemory();

        
		int _getNumBuffersByTag( int tag );
        size_t _getUsedMemoryByTag( int tag );
        double _getMemoryUsageByTag( int tag );

        
	public:
		
		// Function for the main thread of memory
		void runThread();        

        // get xml information
        void getInfo( std::ostringstream& output);

	};
	
};

#endif

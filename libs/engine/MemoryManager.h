/* ****************************************************************************
 *
 * FILE                     MemoryManager.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

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

#include "Buffer.h"					// ss::Buffer
#include "au/Token.h"					// au::Token
#include "au/Stopper.h"				// au::Stopper
#include "au/map.h"					// au::map
#include "au/list.h"					// au::list
#include "au/Format.h"					// au::Format


#include "EngineNotification.h"     // ss:EngineNotificationListener


#define notification_memory_request             "notification_memory_request"
#define notification_memory_request_response    "notification_memory_request_response"

namespace engine {


    class MemoryRequest;
    
	/**
	 
	 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
	 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
	 It is also responsible to manage shared-memory areas to share between differnt processes. 
	 
	 */
	
	class MemoryManager : public NotificationListener
	{
		
		au::Token token;							// Token to protect this instance and memoryRequests
													// It is necessary to protect since network thread can access directly here

		size_t memory;								// Total available memory used
		
		// Monitorization information
		size_t used_memory_input;	    			// Memory used for input
		size_t used_memory_output;	    			// Memory used for output

		int num_buffers_input;						// Number of buffers used as inputs
		int num_buffers_output;						// Number of buffers used as outputs
				
		// List of memory requests
		au::list <MemoryRequest> memoryRequests;	// Only used for inputs
		
		MemoryManager( size_t _memory );
		
	public:

        static void init( size_t _memory );

        static size_t getMemory();
        static size_t getUsedMemory();
        
        static MemoryManager *shared();
        
		~MemoryManager();


		 /*--------------------------------------------------------------------
		 DIRECT mecanish to request buffers
		 --------------------------------------------------------------------*/
		
		/**
		 Interface to get a buffer of memory
		 Used by networkInterface , DataBuffer and FileManager
		 */
		
		Buffer *newBuffer( std::string name ,  size_t size , Buffer::BufferType type );

	private:
		
		Buffer *_newBuffer( std::string name ,  size_t size , Buffer::BufferType type );

        // Check the pending memory requests
        void checkMemoryRequests();
        
	public:
		
		/**
		 Interface to destroy a buffer of memory
		 */
		void destroyBuffer( Buffer *b );
		
		
        /* 
         Function to receive notifications (memory requests)
         */
        
        void notify( Notification* notification );
		
	public:
        
		/*--------------------------------------------------------------------
		 Get information about memory usage
		 --------------------------------------------------------------------*/
		

		// Get information about current memory usage
		
		size_t getUsedMemoryInput();
		size_t getUsedMemoryOutput();
		
		int getNumBuffers();
		int getNumBuffersInput();
		int getNumBuffersOutput();
		
		double getMemoryUsage();
		double getMemoryUsageInput();
		double getMemoryUsageOutput();

		// Informs about the usage of the output memory ( it is used to block data generators if necessary )
		bool availableMemoryOutput();
		
	public:
		
		// Function for the main thread of memory
		void runThread();


		
        static std::string str();
        
    private:

        std::string _str();

        
        
	};
	
};

#endif

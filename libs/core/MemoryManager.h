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
#include "samsonDirectories.h"		// SAMSON_SETUP_FILE


#include "Buffer.h"					// ss::Buffer
#include "Token.h"					// au::Token
#include "Stopper.h"				// au::Stopper
#include "au_map.h"					// au::map
#include "Format.h"					// au::Format

#include "samson.pb.h"				// network::..

#include "EngineNotification.h"     // ss:EngineNotificationListener


// #define SS_SHARED_MEMORY_KEY_ID	872934	// Not used any more since now, IPC_PRIVATE shared memory can be created

namespace ss {


    class MemoryRequest;
    
	/**
	 
	 Memory manager is a singleton implementation to manager the memory used by any component of SAMSON
	 A unifierd view of the memory is necessary to block some data-generator elements when we are close to the maximum memory
	 It is also responsible to manage shared-memory areas to share between differnt processes. 
	 Shared memory is used to isolate operations from main process
	 
	 */
	
	class MemoryManager : public EngineNotificationListener
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
		
		
	public:
		
		MemoryManager();
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
        
        void notify( EngineNotification* notification );
		
	public:
        
		/*--------------------------------------------------------------------
		 Get information about memory usage
		 --------------------------------------------------------------------*/
		
		// Fill information
		void fill(network::WorkerStatus*  ws);

		// Get information about current memory usage
		
		size_t getMemory();
		
		size_t getUsedMemory();
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
		
	};
	
};

#endif

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
#ifndef _H_PROCESS_WRITER
#define _H_PROCESS_WRITER

#include <iostream>					// std::cout

#include "au/string.h"					// au::Format
#include "au/ErrorManager.h"            // au::ErrorManager

#include "engine/MemoryManager.h"

#include "samson/common/coding.h"				// High level definition of KV_*
#include "samson/module/DataInstance.h"         // samson::DataInstance
#include "samson/module/Operation.h"            // samson::Operation
#include "samson/module/KVWriter.h"				// samson::KVWriter

#include "SharedMemoryItem.h"

#define KV_NODE_SIZE	255
#define KV_NODE_UNASIGNED 4294967295u	// 2^32-1

namespace samson {


	class ProcessIsolated;
    
	/**
	 Class to emit key-values ( for maps , reducers, generators, parsers, etc )
	 */
	
	class ProcessWriter : public KVWriter
	{
		ProcessIsolated * processIsolated;          // Pointer to the processIsolated to emit codes through the pipe
		
		engine::SharedMemoryItem *item;				// Shared memory item used at this side ( fork in the middle )
		
		char * buffer;		// General output buffer
		size_t size;		// General output buffer size
		
		int num_outputs;	// Number of global outputs ( channels of output )
        
		// Minibuffer to serialize
		char *miniBuffer;
		size_t miniBufferSize;
        
        // Collection of output data instances and hash-values to check we are using the rigth DataInstance
        DataInstance **outputKeyDataInstance;
        DataInstance **outputValueDataInstance;
		KeyValueHash* keyValueHash;
        
		// Output
		OutputChannel * channel;
		
		// Node buffers
		NodeBuffer *node;	// Pointer to the entire node set
		uint32 num_nodes;	// Total number of nodes ( to fit inside the shared memory block)
		
		uint32 new_node;	// Identifier of the next free block
	
 	private:
        
		friend class ProcessIsolated;
		ProcessWriter( ProcessIsolated * _processIsolated  );
    
    public:
        
		~ProcessWriter();
		
		/**
		 Public function to emit key and values
		 Called allways from the Process side 
		 */
		
		void emit( int output  , DataInstance *key , DataInstance *value );
        
        // Emit data for a particular output channel
        void internal_emit( int output , int hg , char* data , size_t size );
        
		// Clear the current buffer to submit new key-values
		void clear();

		// Function used to flush the content of the buffer ( it sends a code to the "parent" process to emit througt network )
		void flushBuffer( bool finish );
		
	};
	
	class ProcessTXTWriter : public TXTWriter
	{
		
	public:

		ProcessIsolated * workerTaskItem;	// Pointer to the workTaskItem to emit codes through the pipe
		
		char *data;
		size_t *size;
		size_t max_size;
		
		engine::SharedMemoryItem *item;

	private:
        
		friend class ProcessIsolated;
		ProcessTXTWriter( ProcessIsolated * _workerTaskItem  );

    public:
 		
		void emit( const char * _data , size_t _size);

		void flushBuffer( bool finish );
		
	};

}

#endif

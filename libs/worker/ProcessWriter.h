#ifndef _H_PROCESS_WRITER
#define _H_PROCESS_WRITER

#include "coding.h"					// High level definition of KV_*
#include "samson/DataInstance.h"	// ss::DataInstance
#include "au/Format.h"					// au::Format
#include <iostream>					// std::cout
#include "samson/Operation.h"		// ss::Operation
#include "engine/MemoryManager.h"
#include "samson/KVWriter.h"				// ss::KVWriter
#include "SharedMemoryItem.h"

#define KV_NODE_SIZE	255
#define KV_NODE_UNASIGNED 4294967295u	// 2^32-1

namespace ss {


	class ProcessBase;
    
	/**
	 Class to emit key-values ( for maps , reducers, generators, parsers, etc )
	 */
	
	class ProcessWriter : public KVWriter
	{
		ProcessBase * workerTaskItem;	// Pointer to the workTaskItem to emit codes through the pipe
		
		engine::SharedMemoryItem *item;				// Shared memory item used at this side ( fork in the middle )
		
		char * buffer;		// General output buffer
		size_t size;		// General output buffer size
		
		int num_outputs;	// Number of global outputs ( channels of output )
		int num_servers;	// Number of servers where information is divided
		
		// Minibuffer to serialize
		char *miniBuffer;
		size_t miniBufferSize;
		
		// Output
		OutputChannel * channel;
		
		// Node buffers
		NodeBuffer *node;	// Pointer to the entire node set
		uint32 num_nodes;	// Total number of nodes ( to fit inside the shared memory block)
		
		uint32 new_node;	// Identifier of the next free block
		
	public:
		
		ProcessWriter( ProcessBase * _workerTaskItem  );
		~ProcessWriter();
		
		/**
		 Public function to emit key and values
		 Called allways from the Process side 
		 */
		
		void emit( int output , DataInstance *key , DataInstance *value );

		// Clear the current buffer to submit new key-values
		void clear();

		// Function used to flush the content of the buffer ( it sends a code to the "parent" process to emit througt network )
		void flushBuffer(bool finish);

		
	};
	
	class ProcessTXTWriter : public TXTWriter
	{
		
	public:

		ProcessBase * workerTaskItem;	// Pointer to the workTaskItem to emit codes through the pipe
		
		char *data;
		size_t *size;
		size_t max_size;
		
		engine::SharedMemoryItem *item;
		
		ProcessTXTWriter( ProcessBase * _workerTaskItem  );

		
		void emit( const char * _data , size_t _size);

		void flushBuffer(bool finish);
		
	};

}

#endif

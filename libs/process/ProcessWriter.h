#ifndef _H_PROCESS_WRITER
#define _H_PROCESS_WRITER

#include "coding.h"					// High level definition of KV_*
#include "samson/DataInstance.h"	// ss::DataInstance
#include "Format.h"					// au::Format
#include <iostream>					// std::cout
#include "samson/Operation.h"		// ss::Operation
#include "Process.h"				// ss::Process
#include "MemoryManager.h"
#include "samson/KVWriter.h"				// ss::KVWriter

#define KV_NODE_SIZE	255
#define KV_NODE_UNASIGNED 4294967295u	// 2^32-1

namespace ss {

	struct HashGroupOutput {

		NetworkKVInfo info;		// Basic info of this hg
		uint32 first_node;		// First block with output for this hash gorup
		uint32 last_node;		// Last block with output for this hash gorup
		
		void init()
		{
			info.init();
			first_node = KV_NODE_UNASIGNED;
			last_node = KV_NODE_UNASIGNED;
		}
		
	};	

	/**
	 A channel is the unit of data for a particular server and output
	 */
	
	struct OutputChannel {
		
		NetworkKVInfo info;
		HashGroupOutput hg[ KV_NUM_HASHGROUPS ];
		
		void init()
		{
			info.init();
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
				hg[i].init();
		}
	};
	
	/* 
	 Basic unit of storage for output key-values
	 It is used by a particular output channel in a linked-chain way
	 */
	
	struct NodeBuffer {
		uint8 size;					// Size used inside this minibuffer
		char data[KV_NODE_SIZE];	// Real buffer with data
		uint32 next;				// Identifier of the next buffer
		
		void init()
		{
			size = 0;
			next = KV_NODE_UNASIGNED;
		}
		
		inline size_t write( char *_data , size_t _size )
		{
			size_t available = KV_NODE_SIZE - size;
			
			if( _size > available )
			{
				memcpy(data+size, _data, available);
				size = KV_NODE_SIZE;
				return available;
			}
			else
			{
				memcpy(data+size, _data, _size);
				size += _size;
				return _size;
			}
		}
		
		inline bool isFull()
		{
			return ( size == KV_NODE_SIZE );
		}

		inline void setNext( uint32 _next )
		{
			next = _next;
		}
		
		inline size_t availableSpace()
		{
			return KV_NODE_SIZE - size;
		}
		
	};
	
	
	/**
	 Class to emit key-values ( for maps , reducers, etc )
	 */
	
	class ProcessAssistant;
	class WorkerTaskItemWithOutput;
	
	class ProcessWriter : public KVWriter
	{
		Process* process;			
		ProcessAssistant *processAssistant;
		
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
		uint32 num_nodes;	// Total number of nodes
		
		uint32 new_node;	// Identifier of the next free block
		
	public:
		
		ProcessWriter( int shm_id , int _num_outputs , int _num_servers )
		{
			// Get the assignated shared memory region
			SharedMemoryItem *item = MemoryManager::shared()->getSharedMemory( shm_id );
			
			// Pointer to the process to flush output when full
			process = NULL;
			processAssistant = NULL;
			
			// General output buffer
			buffer = item->data;
			size = item->size;
			
			assert( buffer );
			assert( size > 0);
			
			// Number of outputs
			num_outputs = _num_outputs;
			
			// Number of servers
			num_servers = _num_servers;
			
			// Init the minibuffer
			miniBuffer = (char*) malloc( KV_MAX_SIZE );
			miniBufferSize = 0;

			// Outputs structures placed at the begining of the buffer
			channel = (OutputChannel*) buffer;
			
			// Buffer starts next
			node = (NodeBuffer*) ( buffer + sizeof(OutputChannel) * num_outputs * num_servers );
			num_nodes = ( size - (sizeof(OutputChannel)* num_outputs* num_servers )) / sizeof( NodeBuffer );

			//std::cout << "Size of header: " << au::Format::string( sizeof(OutputChannel) * num_outputs * num_servers ) << std::endl;
			//std::cout << "Number of nodes: " << num_nodes  << "\n";

			// Init the entire output structure
			init();
		}
		
		
		~ProcessWriter()
		{
			// Free minibuffer used to serialize key-value here!!
			free( miniBuffer );
		}
		
		// This class is used both in the Process and ProcessAssistant
		
		void setProcess( Process *_process)
		{
			process = _process;
		}
		
		void setProcessAssistant( ProcessAssistant *_processAssistant)
		{
			processAssistant = _processAssistant;
		}
		
		void init()
		{
			// Init all the outputs
			for (int c = 0 ; c < (num_outputs*num_servers) ; c++)
				channel[c].init();
			
			new_node = 0;

		}

		/**
		 Public function to emit key and values
		 Called allways from the Process side 
		 */
		
		void emit( int output , DataInstance *key , DataInstance *value );
	
		/**
		 Function called form the ProcessAssitant side to create and emit the necessary Buffer to the rigth servers
		 */
		
		void FlushBuffer(  WorkerTaskItemWithOutput *taskItem );

	};
	
}

#endif


#include "ProcessWriter.h"			// Own interface

#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "Packet.h"					// ss::Packet
#include "NetworkInterface.h"		// ss::NetworkInterface
#include "WorkerTaskManager.h"		// ss::WorkerTaskItemWithOutput
#include "WorkerTask.h"				// ss::WorkerTask
#include "ProcessBase.h"			// ss::ProcessBase


namespace ss {

	
	ProcessWriter::ProcessWriter( ProcessBase * _workerTaskItem  )
	{
		// Pointer to the worker task item
		workerTaskItem = _workerTaskItem;
		
		// Get the assignated shared memory region
		item = Engine::shared()->memoryManager.getSharedMemory( workerTaskItem->shm_id );
		
		// General output buffer
		buffer = item->data;
		size = item->size;
		
		//std::cout << "Process Writer with " << size << " bytes\n";
		
		if( !buffer )
			LM_X(1,("Internal error: No buffer in a ProcessWriter"));
		if( size == 0)
			LM_X(1,("Wrong size in a ProcessWriter operation"));
		
		// Number of outputs
		num_outputs = workerTaskItem->num_outputs;
		
		// Number of servers
		num_servers = workerTaskItem->num_servers;
		
		// Init the minibuffer
		miniBuffer = (char*) malloc( KVFILE_MAX_KV_SIZE );
		miniBufferSize = 0;
		
		// Outputs structures placed at the begining of the buffer
		channel = (OutputChannel*) buffer;
		
		if( size < sizeof(OutputChannel)* num_outputs* num_servers )
			LM_X(1,("Wrong size for a ProcessWriter operation"));
		
		// Buffer starts next
		node = (NodeBuffer*) ( buffer + sizeof(OutputChannel) * num_outputs * num_servers );
		num_nodes = ( size - (sizeof(OutputChannel)* num_outputs* num_servers )) / sizeof( NodeBuffer );
	
		// Clear this strucutre to receive new key-values
		clear();
	}
	
	ProcessWriter::~ProcessWriter()
	{
		// Free the map of the shared memory
		Engine::shared()->memoryManager.freeSharedMemory( item );		
		
		// Free minibuffer used to serialize key-value here!!
		free( miniBuffer );
	}
	
	void ProcessWriter::emit( int output , DataInstance *key , DataInstance *value )
	{
		// Serialize to the minibuffer
		
		size_t key_size		= key->serialize( miniBuffer );
		size_t value_size	= value->serialize( miniBuffer + key_size );
		miniBufferSize		= key_size + value_size;
		
		// Emit the miniBuffer to the rigth place
		int hg = key->hash(KVFILE_NUM_HASHGROUPS); 
		
		// Detect the server to sent
		int server = key->hash(num_servers);
		
		// Get a pointer to the current node
		OutputChannel * _channel		= &channel[ output * num_servers + server ];	// Output channel ( output + server )
		HashGroupOutput * _hgOutput		= &_channel->hg[hg];							// Current hash-group output
		
		size_t availableSpace = (num_nodes - new_node)*KV_NODE_SIZE;
		if( _hgOutput->last_node != KV_NODE_UNASIGNED )
			availableSpace += node[ _hgOutput->last_node ].availableSpace( );
		
		//std::cout << "Emiting " << miniBufferSize << " bytes. Available: " << availableSpace << "\n";
		
		// Check if it will fit
		if( miniBufferSize >= availableSpace )
		{
			// Process the output buffer and clear to continue
			flushBuffer(false);
			clear();
		}
		
		// Update the info in the particular output and the concrete hash-group 
		_channel->info.append(	miniBufferSize , 1 );
		_hgOutput->info.append( miniBufferSize , 1 ); 
		
		uint32 miniBufferPos = 0;
		
		// Get a pointer to the first node ( or create if not created before )
		NodeBuffer *_node;
		if(  _hgOutput->last_node == KV_NODE_UNASIGNED )
		{
			if( new_node >= num_nodes )
				LM_X(1,("Internal error"));

			node[new_node].init();				// Init the new node
			_hgOutput->first_node = new_node;	// Update the HasgGroup structure to point here
			_hgOutput->last_node = new_node;	// Update the HasgGroup structure to point here
			_node = &node[new_node];			// Point to this one to write
			new_node++;
		}
		else
		{
			if( _hgOutput->last_node >= num_nodes )
				LM_X(1,("Internal error"));

			_node = &node[ _hgOutput->last_node ];				// Current write node
		}
		
		// Fill following nodes...
		while( miniBufferPos < miniBufferSize )
		{
			
			// Write in the node
			miniBufferPos += _node->write( miniBuffer + miniBufferPos , miniBufferSize - miniBufferPos );
			
			if( _node->isFull() )
			{
				_node->setNext( new_node );			// Set the next in my last node
				node[new_node].init();				// Init the new node
				_hgOutput->last_node = new_node;	// Update the HasgGroup structure to point here
				if( new_node > num_nodes )
					LM_X(1,("Internal error"));

				_node = &node[new_node];			// Point to this one to write
				new_node++;
			}
		}
	}
	
	void ProcessWriter::flushBuffer(bool finish)
	{
		// Send code to be understoo
        if ( finish ) 
            workerTaskItem->sendCode( WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH );
        else
            workerTaskItem->sendCode( WORKER_TASK_ITEM_CODE_FLUSH_BUFFER );
		
		// Clear the buffer
		clear();
		
	}

	void ProcessWriter::clear()
	{
		// Init all the outputs
		for (int c = 0 ; c < (num_outputs*num_servers) ; c++)
			channel[c].init();
		new_node = 0;
	}
	

#pragma mark ProcessTXTWriter
	
	
	ProcessTXTWriter::ProcessTXTWriter( ProcessBase * _workerTaskItem  )
	{
		workerTaskItem = _workerTaskItem;
		
		// Get the assignated shared memory region
		item = Engine::shared()->memoryManager.getSharedMemory( workerTaskItem->shm_id );
		
		// Size if the firt position in the buffer 
		size = (size_t*) item->data;
		
		// Init the data buffer used here	
		data = item->data + sizeof(size_t);
		max_size = item->size - sizeof(size_t); // This is the available space int he buffer

		// Init the size of the output
		*size = 0;
	}
	
	void ProcessTXTWriter::flushBuffer(bool finish)
	{
		// Send code to be understoo
        if ( finish ) 
            workerTaskItem->sendCode( WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH );
        else
            workerTaskItem->sendCode( WORKER_TASK_ITEM_CODE_FLUSH_BUFFER );
		
        
		// Clear the buffer
		*size = 0;
	}
	
	void ProcessTXTWriter::emit( const char * _data , size_t _size)
	{
		
		if( *size + _size  > max_size )
			flushBuffer(false);
		
		memcpy(data+ (*size), _data, _size);
		*size += _size;
	}
	
}

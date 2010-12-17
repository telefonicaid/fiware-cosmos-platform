
#include "ProcessWriter.h"			// Own interface
#include <assert.h>					// assert(.)

#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "Packet.h"					// ss::Packet
#include "NetworkInterface.h"		// ss::NetworkInterface
#include "ProcessAssistant.h"		// ss::ProcessAssistant
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "WorkerTaskManager.h"		// ss::WorkerTaskItemWithOutput
#include "WorkerTask.h"				// ss::WorkerTask
#include "WorkerTaskItem.h"			// ss::WorkerTaskItem
namespace ss {

	void ProcessWriter::emit( int output , DataInstance *key , DataInstance *value )
	{
		// Serialize to the minibuffer
		
		size_t key_size		= key->serialize( miniBuffer );
		size_t value_size	= value->serialize( miniBuffer + key_size );
		miniBufferSize		= key_size + value_size;
		
		// Emit the miniBuffer to the rigth place
		ss_hg hg = key->hash(KV_NUM_HASHGROUPS); 
		
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
			/*
			 Emit function can be called only from the process side.
			 If the output buffer is full, a message is sent to the Process Assistant to flush the buffer to the "Network"
			 After that, "process" can continue reseting the output buffer to fill it again
			 */

			assert( process );	
			process->processOutput( );
			init();
		}
		
		// Update the info in the particular output and the concrete hash-group 
		_channel->info.append(	miniBufferSize , 1 );
		_hgOutput->info.append( miniBufferSize , 1 ); 
		
		uint32 miniBufferPos = 0;
		
		// Get a pointer to the first node ( or create if not created before )
		NodeBuffer *_node;
		if(  _hgOutput->last_node == KV_NODE_UNASIGNED )
		{
			node[new_node].init();				// Init the new node
			_hgOutput->first_node = new_node;	// Update the HasgGroup structure to point here
			_hgOutput->last_node = new_node;	// Update the HasgGroup structure to point here
			_node = &node[new_node];			// Point to this one to write
			new_node++;
		}
		else
			_node = &node[ _hgOutput->last_node ];				// Current write node
		
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
				_node = &node[new_node];			// Point to this one to write
				new_node++;
			}
		}
	}

	
	// Create as many network messages to send all this key-values to all the workers

	void ProcessWriter::FlushBuffer( WorkerTaskItem *taskItem )
	{
		size_t task_id = taskItem->task->task_id;
		
		for (int o = 0 ; o < num_outputs ; o++)
		{
			// Name of the queue to sent this packet ( if any )
			network::Queue output_queue = taskItem->getOutputQueue( o );
			
			for (int s = 0 ; s < num_servers ; s++)
			{				
				OutputChannel * _channel = &channel[ o * num_servers + s ];	

				if( _channel->info.size > 0)
				{
					Buffer *buffer = MemoryManager::shared()->newBuffer( "ProcessWriter", NETWORK_TOTAL_HEADER_SIZE + _channel->info.size );
					assert( buffer );

					// Pointer to the header
					NetworkHeader *header = (NetworkHeader*) buffer->getData();
					
					// Pointer to the info vector
					NetworkKVInfo *info = (NetworkKVInfo*) (buffer->getData() + sizeof( NetworkHeader ));
					
					// Initial offset for the buffer to write data
					buffer->skipWrite(NETWORK_TOTAL_HEADER_SIZE);
					
					header->init();
					header->setInfo(  _channel->info );	// Global information of this buffer
					header->setFormat( KVFormat( output_queue.format().keyformat() , output_queue.format().valueformat() ) );
					
					for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
					{
						HashGroupOutput * _hgOutput	= &_channel->hg[i];							// Current hash-group output

						// Set gloal info
						info[i] = _hgOutput->info;
						
						// Write data followign nodes
						uint32 node_id = _hgOutput->first_node;

						while( node_id != KV_NODE_UNASIGNED )
						{
							bool ans = buffer->write( (char*) node[node_id].data, node[node_id].size );
							assert( ans );
							
							// Go to the next node
							node_id = node[node_id].next;
						}
					}
					
					assert( buffer->getSize() == buffer->getMaxSize() );

					// Sent this buffer to somewhere
					assert( processAssistant );

					Packet *p = new Packet();
					p->buffer = buffer;
					network::WorkerDataExchange *dataMessage =  p->message.mutable_data();
					
					dataMessage->set_task_id(task_id);
					dataMessage->mutable_queue( )->CopyFrom( output_queue );
											
					NetworkInterface *network = processAssistant->taskManager->worker->network;
					network->send(processAssistant->taskManager->worker, network->workerGetIdentifier(s) , Message::WorkerDataExchange, p);
					
				}
			}
			
		}
		
	}


	
	
	void ProcessTXTWriter::FlushBuffer( WorkerTaskItem *taskItem )
	{
		
		if( *size > 0)
		{
			
			size_t task_id = taskItem->task->task_id;
			
			assert( processAssistant );
			
			Buffer *buffer = MemoryManager::shared()->newBuffer( "ProcessTXTWriter", *size );
			assert( buffer );

			// There is only one output queue
			network::Queue output_queue = taskItem->getOutputQueue( 0 );

			
			// copy the entire buffer to here
			memcpy(buffer->getData(), data, *size);
			buffer->setSize(*size);
			
			Packet *p = new Packet();
			p->buffer = buffer;
			network::WorkerDataExchange *dataMessage =  p->message.mutable_data();
			
			dataMessage->set_task_id(task_id);
			dataMessage->mutable_queue( )->CopyFrom( output_queue );
			dataMessage->set_txt(true);
			NetworkInterface *network = processAssistant->taskManager->worker->network;
			network->send(processAssistant->taskManager->worker, network->getMyidentifier() , Message::WorkerDataExchange, p);
		}
	}	
	
	
}

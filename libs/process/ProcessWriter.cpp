
#include "ProcessWriter.h"			// Own interface
#include <assert.h>					// assert(.)

#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "Packet.h"					// ss::Packet
#include "NetworkInterface.h"		// ss::NetworkInterface
#include "ProcessAssistant.h"		// ss::ProcessAssistant
#include "SamsonWorker.h"			// ss::SamsonWorker


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
		
		// Check if it will fit
		if( miniBufferSize >= availableSpace )
		{
			assert( process );	// This is called from the Process side
			// Free buffer
			//process->XXX
			init();
		}
		
		// Update the info in the particular output and the concrete hash-group 
		_channel->info.update(	1, miniBufferSize );
		_hgOutput->info.update( 1, miniBufferSize ); 
		
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



	void ProcessWriter::FlushBuffer()
	{
		std::cout << "Debuggin ProcessWriter \n";
		
		for (int o = 0 ; o < num_outputs ; o++)
		{
			for (int s = 0 ; s < num_servers ; s++)
			{				
				OutputChannel * _channel		= &channel[ o * num_servers + s ];	

				if( _channel->info.size > 0)
				{
				
					Buffer *buffer = MemoryManager::shared()->newBuffer( KV_HASH_GROUP_VECTOR_SIZE_NETWORK + _channel->info.size );
					assert( buffer );
					
					// Pointer to the begining for info vector
					NetworkKVInfo *info = (NetworkKVInfo*) buffer->getData();
					
					// Initial offset for the buffer
					buffer->skip(KV_HASH_GROUP_VECTOR_SIZE_NETWORK);
					
					info[0] = _channel->info;	// Global info
					
					for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
					{
						HashGroupOutput * _hgOutput	= &_channel->hg[i];							// Current hash-group output

						// Set gloal info
						info[i+1] = _hgOutput->info;
						
						// Write data followign nodes
						uint32 node_id = _hgOutput->first_node;

						while( node_id != KV_NODE_UNASIGNED )
						{
							bool ans = buffer->write( (char*)&node[node_id].data, node[node_id].size );
							assert( ans );
							
							// Go to the next node
							node_id = node[node_id].next;
						}
					}
					
					assert( buffer->getSize() == buffer->getMaxSize() );

					// Sent this buffer to somewhere

					if( processAssistant )
					{
						Packet p;
						// TODO: Prepare the messsage
						p.buffer = buffer;
						
						NetworkInterface *network = processAssistant->worker->network;
						network->send(processAssistant->worker, network->workerGetIdentifier(s) , Message::WorkerDataExchange, &p);
					}
					else
					{
						//std::cout << "Output " << o << " Server " << s << " : "<< au::Format::string( buffer->getSize() ) << " bytes\n";
						std::cout << "Output " << o << " Server " << s << " : " << au::Format::string( _channel->info.kvs ) << " kvs in " << au::Format::string( buffer->getSize() - KV_HASH_GROUP_VECTOR_SIZE_NETWORK ) << " real bytes\n";
						MemoryManager::shared()->destroyBuffer( buffer );
					}

					
					
				}
			}
			
		}
	}

}

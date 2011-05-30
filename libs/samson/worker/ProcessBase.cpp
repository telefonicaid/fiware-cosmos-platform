

#include "ProcessBase.h"			// Own interface
#include "WorkerTask.h"				// samson::WorkerTask
#include "WorkerTaskManager.h"		// samson::WorkerTaskManager
#include "samson/common/coding.h"					// All data definitions
#include "samson/network/Packet.h"					// samson::Packet
#include "samson/common/EnvironmentOperations.h"	// copyEnviroment
#include "WorkerTask.h"				// samson::WorkerTask
#include "samson/worker/SamsonWorker.h"			// samson::SamsonWorker
#include "samson/isolated/SharedMemoryItem.h"       // samson::SharedMemoryItem
#include "logMsg/logMsg.h"                 // LM_X
#include "samson/common/MemoryTags.h"             // MemoryOutputNetwork

namespace samson {
    
	
#pragma mark ProcessItemKVGenerator
	
	ProcessBase::ProcessBase( WorkerTask *task , ProcessBaseType _type ) 
        : ProcessIsolated( task->workerTask.output_queue_size() , task->workerTask.servers() )
	{
		// Pointer to the task in task manager
		//task = _task;
		type = _type;
		
		// copy the message received from the controller
		workerTask = new network::WorkerTask();
		workerTask->CopyFrom( task->workerTask );
		
		// Get the pointer to the network interface to send messages
		network = task->taskManager->worker->network;
		
		// Get the task_id
		task_id = task->workerTask.task_id();

									
		copyEnviroment( task->workerTask.environment() , &environment ); 
		
		
		workerTaskManager = task->taskManager;
        worker = task->taskManager->worker->network->getWorkerId();    // Get the worker id information
        hg_set = 0; // Default hg_set
        
	}
	
	ProcessBase::~ProcessBase()
	{
        // Remove local copy of the workerTask message
        delete workerTask;
	}
	
	void ProcessBase::runIsolated()
	{
		switch (type) {
			case key_value:
				runIsolatedKV();
				break;
			case txt:
				runIsolatedTXT();
				break;
		}
		
	}
	
	void ProcessBase::runIsolatedTXT()
	{
		ProcessTXTWriter *writer = new ProcessTXTWriter( this );
		
		// Generate the key-values
		generateTXT( writer );
		
		writer->flushBuffer(true);
		delete writer;
	}
	
	void ProcessBase::runIsolatedKV()
	{
		ProcessWriter *writer = new ProcessWriter( this );
		
		// Generate the key-values
		generateKeyValues( writer );
		
		writer->flushBuffer(true);
		
		delete writer;
		
	}

	void ProcessBase::flushBuffer( bool finish )
	{
		switch (type) {
			case key_value:
				flushKVBuffer(finish);
				break;
			case txt:
				flushTXTBuffer(finish);
				break;
		}
		
	}

	void ProcessBase::flushKVBuffer( bool finish )
	{
		
		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */
		
        while( !isReady() )
			halt();
		
#pragma mark ---		
		
		
		// General output buffer
		char * buffer = item->data;
		size_t size = item->size;
		
		// Make sure everything is correct
		if( !buffer )
			LM_X(1,("Internal error: Missing buffer in ProcessBase"));
		if( size == 0)
			LM_X(1,("Internal error: Wrong size for ProcessBase"));

		
		// Outputs structures placed at the begining of the buffer
		OutputChannel *channel = (OutputChannel*) buffer;
		
		// Buffer starts next
		NodeBuffer* node = (NodeBuffer*) ( buffer + sizeof(OutputChannel) * num_outputs * num_workers );
		//size_t num_nodes = ( size - (sizeof(OutputChannel)* num_outputs* num_servers )) / sizeof( NodeBuffer );
		
#pragma mark ---
		
		//size_t task_id = task->workerTask.task_id();
		
		for (int o = 0 ; o < num_outputs ; o++)
		{
			// Name of the queue to sent this packet ( if any )
			//network::Queue output_queue = task->workerTask.output( o );
			network::Queue output_queue = workerTask->output_queue( o ).queue();
			
			
			for (int s = 0 ; s < num_workers ; s++)
			{				
				OutputChannel * _channel = &channel[ o * num_workers + s ];	
				
				if( _channel->info.size > 0)
				{
                    engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "ProcessWriter", KVFILE_TOTAL_HEADER_SIZE + _channel->info.size , MemoryOutputNetwork );
					if( !buffer )
						LM_X(1,("Internal error: Missing buffer in ProcessBase"));
					
					// Pointer to the header
					KVHeader *header = (KVHeader*) buffer->getData();
					
					// Pointer to the info vector
					KVInfo *info = (KVInfo*) (buffer->getData() + sizeof( KVHeader ));
					
					// Initial offset for the buffer to write data
					buffer->skipWrite(KVFILE_TOTAL_HEADER_SIZE);

					KVFormat format = KVFormat( output_queue.format().keyformat() , output_queue.format().valueformat() );
					header->init( format , _channel->info );
					
					for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
					{
						HashGroupOutput * _hgOutput	= &_channel->hg[i];							// Current hash-group output
						
						// Set gloal info
						info[i] = _hgOutput->info;
						
						// Write data followign nodes
						uint32 node_id = _hgOutput->first_node;
						
						while( node_id != KV_NODE_UNASIGNED )
						{
							bool ans = buffer->write( (char*) node[node_id].data, node[node_id].size );
							if( !ans )
								LM_X(1,("Error writing key-values into a temporal Buffer"));
							
							// Go to the next node
							node_id = node[node_id].next;
						}
					}
					
					if( buffer->getSize() != buffer->getMaxSize() )
						LM_X(1,("Internal error"));

					
					// Create packet for this output
					
					Packet *p = new Packet( Message::WorkerDataExchange );
					p->buffer = buffer;
					network::WorkerDataExchange *dataMessage =  p->message->mutable_data();
					
					dataMessage->set_task_id(task_id);
                    dataMessage->set_txt( false );
					dataMessage->mutable_queue( )->CopyFrom( output_queue );
                    dataMessage->set_worker( worker );
                    dataMessage->set_hg_set( hg_set );
                    dataMessage->set_finish( finish );
					
					network->send(NULL, network->workerGetIdentifier(s) , p);
					
				}
			}
			
		}
		
	}	

	
	void ProcessBase::flushTXTBuffer( bool finish )
	{

		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */
		
        while( !isReady() )
			halt();
		
#pragma mark ---		
		
		
		// Size if the firt thing in the buffer
		size_t *size = (size_t*) item->data;
		
		// Init the data buffer used here	
		char *data = item->data + sizeof(size_t);
		
#pragma mark ---		
		
		if( *size > 0 )
		{
			
			//size_t task_id = task->workerTask.task_id();
			
			engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "ProcessTXTWriter", *size , MemoryOutputNetwork );
			if( !buffer )
				LM_X(1,("Internal error"));

			
			// There is only one output queue
			//network::Queue output_queue = task->workerTask.output( 0 );
			if( workerTask->output_queue_size() == 0)
			  LM_X(1,("Internal error: WorkerTask message without output queue"));

			network::Queue output_queue = workerTask->output_queue( 0 ).queue();
			
			// copy the entire buffer to here
			memcpy(buffer->getData(), data, *size);
			buffer->setSize(*size);
			
			Packet *p = new Packet( Message::WorkerDataExchange );
			p->buffer = buffer;
			network::WorkerDataExchange *dataMessage =  p->message->mutable_data();
			
			dataMessage->set_task_id(task_id);
			dataMessage->mutable_queue( )->CopyFrom( output_queue );
			dataMessage->set_txt(true);
            dataMessage->set_worker( worker );
            dataMessage->set_hg_set( hg_set );
            dataMessage->set_finish( finish );
			
			network->send(NULL, network->getMyidentifier() , p);
		}
		
	}		
	

}

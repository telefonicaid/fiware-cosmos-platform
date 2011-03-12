

#include "ProcessBase.h"			// Own interface
#include "WorkerTask.h"				// ss::WorkerTask
#include "WorkerTaskManager.h"		// ss::WorkerTaskManager
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"
#include "coding.h"					// All data definitions
#include "Packet.h"					// ss::Packet
#include "EnvironmentOperations.h"	// copyEnviroment
#include "WorkerTask.h"				// ss::WorkerTask
#include "SamsonWorker.h"			// ss::SamsonWorker

namespace ss {
	
	
#pragma mark ProcessItemKVGenerator
	
	ProcessBase::ProcessBase( WorkerTask *task , ProcessBaseType _type )
	{
		// Pointer to the task in task manager
		//task = _task;
		type = _type;
		
		num_outputs = task->workerTask.output_queue_size();
		num_servers = task->workerTask.servers();

		// copy the message received from the controller
		workerTask = new network::WorkerTask();
		workerTask->CopyFrom( task->workerTask );
		
		// Get the pointer to the network interface to send messages
		network = task->taskManager->worker->network;
		
		// Get the task_id
		task_id = task->workerTask.task_id();

									
		copyEnviroment( task->workerTask.environment() , &environment ); 
		
		item = NULL; // Initialized at init function

									
									
	}
	
	ProcessBase::~ProcessBase()
	{
		
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
		
		writer->flushBuffer();
		delete writer;
	}
	
	void ProcessBase::runIsolatedKV()
	{
		ProcessWriter *writer = new ProcessWriter( this );
		
		// Generate the key-values
		generateKeyValues( writer );
		
		writer->flushBuffer();
		
		delete writer;
		
	}

	void ProcessBase::flushBuffer( )
	{
		switch (type) {
			case key_value:
				flushKVBuffer();
				break;
			case txt:
				flushTXTBuffer();
				break;
		}
	}

	void ProcessBase::flushKVBuffer( )
	{
		
		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */
		
		if( MemoryManager::shared()->getMemoryUsageOutput() >= 1.0)
			halt();
		
#pragma mark ---		
		
		
		// General output buffer
		char * buffer = item->data;
		size_t size = item->size;
		
		// Make sure everything is correct
		assert( buffer );
		assert( size > 0);
		
		// Outputs structures placed at the begining of the buffer
		OutputChannel *channel = (OutputChannel*) buffer;
		
		// Buffer starts next
		NodeBuffer* node = (NodeBuffer*) ( buffer + sizeof(OutputChannel) * num_outputs * num_servers );
		//size_t num_nodes = ( size - (sizeof(OutputChannel)* num_outputs* num_servers )) / sizeof( NodeBuffer );
		
#pragma mark ---
		
		//size_t task_id = task->workerTask.task_id();
		
		for (int o = 0 ; o < num_outputs ; o++)
		{
			// Name of the queue to sent this packet ( if any )
			//network::Queue output_queue = task->workerTask.output( o );
			network::Queue output_queue = workerTask->output_queue( o ).queue();
			
			
			for (int s = 0 ; s < num_servers ; s++)
			{				
				OutputChannel * _channel = &channel[ o * num_servers + s ];	
				
				if( _channel->info.size > 0)
				{
					Buffer *buffer = MemoryManager::shared()->newBuffer( "ProcessWriter", KVFILE_TOTAL_HEADER_SIZE + _channel->info.size , Buffer::output );
					assert( buffer );
					
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
							assert( ans );
							
							// Go to the next node
							node_id = node[node_id].next;
						}
					}
					
					assert( buffer->getSize() == buffer->getMaxSize() );
					
					// Create packet for this output
					
					Packet *p = new Packet();
					p->buffer = buffer;
					network::WorkerDataExchange *dataMessage =  p->message.mutable_data();
					
					dataMessage->set_task_id(task_id);
					dataMessage->mutable_queue( )->CopyFrom( output_queue );
					
					network->send(NULL, network->workerGetIdentifier(s) , Message::WorkerDataExchange, p);
					
				}
			}
			
		}
		
		
	}	

	
	void ProcessBase::flushTXTBuffer(  )
	{
		
		/*
		 After flushing we check that available memory is under 100%.
		 Otherwise we halt notifying this to the ProcessManager
		 */
		
		if( MemoryManager::shared()->getMemoryUsageOutput() >= 1.0)
			halt();
		
#pragma mark ---		
		
		
		// Size if the firt thing in the buffer
		size_t *size = (size_t*) item->data;
		
		// Init the data buffer used here	
		char *data = item->data + sizeof(size_t);
		
#pragma mark ---		
		
		
		if( *size > 0)
		{
			
			//size_t task_id = task->workerTask.task_id();
			
			Buffer *buffer = MemoryManager::shared()->newBuffer( "ProcessTXTWriter", *size , Buffer::output );
			assert( buffer );
			
			// There is only one output queue
			//network::Queue output_queue = task->workerTask.output( 0 );
			network::Queue output_queue = workerTask->output_queue( 0 ).queue();
			
			// copy the entire buffer to here
			memcpy(buffer->getData(), data, *size);
			buffer->setSize(*size);
			
			Packet *p = new Packet();
			p->buffer = buffer;
			network::WorkerDataExchange *dataMessage =  p->message.mutable_data();
			
			dataMessage->set_task_id(task_id);
			dataMessage->mutable_queue( )->CopyFrom( output_queue );
			dataMessage->set_txt(true);
			
			network->send(NULL, network->getMyidentifier() , Message::WorkerDataExchange, p);
		}
	}		
	
	
	/*
	 
	 void WorkerTaskItem::setTaskAndItemId( WorkerTask *_task , int _itemId )
	 {
	 task = _task;
	 item_id = _itemId;
	 }
	 
	 WorkerTaskItem::State WorkerTaskItem::getState()
	 {
	 return state;
	 }
	 
	 // Funciton to get the run-time status of this object
	 void WorkerTaskItem::getStatus( std::ostream &output , std::string prefix_per_line )
	 {
	 output << "Item " << item_id <<  " (Task: " << task->task_id << ") ";
	 output << "[SHM: " << shm_input << "] ";
	 switch (state) {
	 case definition:
	 output << " in definition ";
	 break;
	 case ready_to_run:
	 output << " ready to run ";
	 break;
	 case no_memory:
	 output << " waiting available memory ";
	 break;
	 case loading_inputs:
	 output << " loading [" << confirmed_input_files << "/" << num_input_files << "]";
	 break;
	 case ready_to_load_inputs:
	 output << " ready to load inputs ";
	 break;
	 case running:
	 output << " running ";
	 break;
	 }
	 
	 output << "\n";
	 }
	 
	 std::string WorkerTaskItem::getStatus()
	 {
	 
	 std::ostringstream output;
	 output << "(";
	 switch (state) {
	 case definition:
	 output << "D";
	 break;
	 case ready_to_run:
	 output << "R";
	 break;
	 case no_memory:
	 output << "M";
	 break;
	 case loading_inputs:
	 output << "L [" << confirmed_input_files << "/" << num_input_files << "]";
	 break;
	 case ready_to_load_inputs:
	 output << "RL";
	 break;
	 case running:
	 output << "Run";
	 break;
	 }
	 
	 output << ":" << shm_input << ")";
	 return output.str();
	 
	 }
	 
	 
	 
	 void WorkerTaskItem::addInputFiles( FileManagerReadItem *item )
	 {
	 lock.lock();
	 
	 item->setDelegate( this );	// Make sure task manager is the delegate for all inputs 
	 FileManager::shared()->addItemToRead( item );
	 num_input_files++;
	 
	 lock.unlock();
	 }
	 
	 void WorkerTaskItem::fileManagerNotifyFinish(size_t id, bool success)
	 {
	 lock.lock();
	 
	 confirmed_input_files++;
	 
	 lock.unlock();
	 
	 if( confirmed_input_files == num_input_files)
	 {
	 state = ready_to_run;
	 
	 // Wake up task manager process
	 task->taskManager->wakeUp();
	 }
	 
	 }
	 
	 
	 
	 bool WorkerTaskItem::isReadyToRun()
	 {
	 return( state == ready_to_run );
	 }
	 
	 void WorkerTaskItem::setError( std::string _error_message )
	 {
	 error = true;
	 error_message = _error_message;
	 }
	 
	 void WorkerTaskItem::start()
	 {
	 assert( state == ready_to_run);
	 state = running;
	 }
	 
	 void WorkerTaskItem::setup()
	 {
	 assert( task );
	 
	 if (state == definition)
	 state = no_memory;		// allways go the next level
	 
	 if( state == no_memory )
	 {
	 shm_input = MemoryManager::shared()->getFreeSharedMemory();
	 if( shm_input != -1)
	 state = ready_to_load_inputs;
	 }
	 
	 if( state == ready_to_load_inputs)
	 {
	 setupInputs();					// Prepare all the inputs to be read
	 
	 if(  confirmed_input_files == num_input_files )		// Theoretically, if no input is finally scheduled
	 state = ready_to_run;
	 else
	 state = loading_inputs;
	 }
	 
	 }
	 
	 void WorkerTaskItem::freeResources()
	 {
	 if( shm_input != -1)
	 MemoryManager::shared()->freeSharedMemory( shm_input );
	 }
	 
	 network::Queue WorkerTaskItem::getOutputQueue(int o)
	 {
	 assert( workerTask.output_size() > o );
	 return workerTask.output(o);
	 }
	 
	 
	 
	 
	 */
	
}

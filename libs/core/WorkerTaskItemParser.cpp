
#include "WorkerTaskItemParser.h"		// Own interface
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "WorkerTask.h"					// ss::WorkerTask
#include "SamsonSetup.h"				// ss::SamsonSetup

namespace ss {

	void WorkerTaskItemOperation::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		Operation * op = pa->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		
		//p.set_input_size( XXX );			// For Parse operation
		
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		
		p.set_input_shm( shm_input );						// Set the input shared memory buffer
		
		p.mutable_environment()->CopyFrom( task->environment );
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}
	
	
	void WorkerTaskItemOperation::setupInputs()
	{
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		size_t size = FileManagerReadItem::sizeOfFile( fileName );
		
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , smi->data , task->taskManager );
		addInputFiles( item );
		
		// The delegate is TaskManager: it received notifications form the FileManager and unblock items
		
	}
	

#pragma mark WorkerTaskItemReduceOrganizer
	
	
	WorkerTaskItemReduceOrganizer::WorkerTaskItemReduceOrganizer( WorkerTask * _parentTask, const network::WorkerTask & _workerTask )
	{
		parentTask = _parentTask;	// Keep a pointer to the parent task to add new items
		workerTask = _workerTask;	// Copy all the information contained in the message
		
		int num_inputs = workerTask.input_size();
		int *num_input_files = (int*) malloc( sizeof(int) * num_inputs );
		for( int i = 0 ; i < num_inputs ; i++)
			num_input_files[i] = workerTask.input(i).file_size();
		
		// Prepare the structure to store information
		parentTask->reduceInformation = new ReduceInformation( NULL,  num_inputs , num_input_files , KV_NUM_HASHGROUPS + 1 );
		
		free( num_input_files );
		
		
		// It is no necessary to have a shared memory area
		state = ready_to_load_inputs;
		
	}
	
	
	void WorkerTaskItemReduceOrganizer::setupInputs()
	{
		// Order all the reads for all the input files
		
		for (int i = 0 ; i < parentTask->reduceInformation->num_inputs ; i++)
		{
			for (int f = 0 ; f < parentTask->reduceInformation->num_input_files[i] ; f++)
			{
				std::string fileName = workerTask.input(i).file(f).name();
				size_t offset = sizeof( FileHeader );
				size_t size = KV_HASH_GROUP_VECTOR_SIZE_FILE;
			
				FileManagerReadItem *item = new FileManagerReadItem( fileName , offset , size , (char *) parentTask->reduceInformation->info[i][f] , task->taskManager );
				addInputFiles( item );
			}
			
		}

		
	}
	
	void WorkerTaskItemReduceOrganizer::run( ProcessAssistant *pa )
	{
		// Organize the reduce in multiple WorkerTaskItems to process each set of hash-groups
		
		int	item_hg_begin = 0;	// Starting at hash-group
		int item_hg_end	 = 0;	// End hashgroup

		size_t item_size = 0;	// Total size in bytes to read
		
		size_t max_item_size = SamsonSetup::shared()->shared_memory_size_per_buffer * 0.7;
		int max_num_hgs = KV_NUM_HASHGROUPS / 10;	// Minimum 10 divisions for force multicore approach
		
		//std::cout << "Reduce organization: max_size=" << au::Format::string(max_item_size) << " max_num_hgs=" << max_num_hgs << std::endl;
		
		for (int hg = 0 ; hg < KV_NUM_HASHGROUPS ; hg++)
		{
			
			size_t size_of_this_hg = 0;
			for (int i = 0 ; i < parentTask->reduceInformation->num_inputs ; i++)
				for (int f = 0 ; f < parentTask->reduceInformation->num_input_files[i] ; f++)
					size_of_this_hg += parentTask->reduceInformation->info[i][f][hg+1].size;

			if( ( size_of_this_hg + item_size > max_item_size ) || ( (hg - item_hg_begin ) > max_num_hgs ) )
			{
				item_hg_end = hg;
				
				// Create a new item
				//std::cout << "New item " << item_hg_begin << " -> " << item_hg_end << " Size: " << au::Format::string( item_size ) << std::endl;
				
				parentTask->addItem( new WorkerTaskItemReduce( item_hg_begin , item_hg_end ,this,  workerTask ) );
				
				// Prepare for the next one
				item_hg_begin = hg;
				item_size = size_of_this_hg;
			}
			else
			{
				item_size += size_of_this_hg;
			}
			
		}
		
		// Create one with the last group
		item_hg_end = KV_NUM_HASHGROUPS;
		parentTask->addItem( new WorkerTaskItemReduce( item_hg_begin , item_hg_end ,this,  workerTask ) );
		
		
	}
	
	
#pragma mark WorkerTaskItemReduce
	
	void WorkerTaskItemReduce::setupInputs()
	{
		
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		char* data = smi->data;

		
		// Copy KVInfo to the shard memory area
		int file_counter=0;
		size_t size_info = sizeof(FileKVInfo) * num_hash_groups;
		for (int i = 0 ; i < parentTask->reduceInformation->num_inputs ; i++)
			for (int f = 0 ; f < parentTask->reduceInformation->num_input_files[i] ; f++)
			{
				memcpy(data + file_counter * size_info , &parentTask->reduceInformation->info[i][f][hg_begin+1], size_info);
				file_counter++;
			}

		char *file_data = data + file_counter* size_info;
		
		for (int i = 0 ; i < parentTask->reduceInformation->num_inputs ; i++)
			for (int f = 0 ; f < parentTask->reduceInformation->num_input_files[i] ; f++)
			{
				std::string fileName = workerTask.input(i).file(f).name();
				
				// Compute the offset and size for this file and offset
				size_t offset = KV_TOTAL_FILE_HEADER_SIZE;
				
				for (int hg = 0 ; hg < hg_begin ; hg++)
					offset+= parentTask->reduceInformation->info[i][f][hg+1].size;
				
				size_t size = 0;

				for (int hg = hg_begin ; hg < hg_end ; hg++)
					size+= parentTask->reduceInformation->info[i][f][hg+1].size;
				
				// Write first size
				*((size_t*)file_data) = size;
				file_data += sizeof( size_t );
				
				FileManagerReadItem *item = new FileManagerReadItem( fileName , offset , size , file_data , task->taskManager );
				addInputFiles( item );

				file_data += size;
			}
		
	}
	
	void WorkerTaskItemReduce::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		std::string operation = workerTask.operation();
		
		Operation * op = pa->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
				
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		
		p.set_input_shm( shm_input );						// Set the input shared memory buffer

		p.mutable_worker_task()->CopyFrom( workerTask );
		
		p.mutable_environment()->CopyFrom( task->environment );
		
		p.set_num_hash_groups( num_hash_groups );
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;	}
	
	
	
	
}
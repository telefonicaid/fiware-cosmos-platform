
#include "WorkerTaskItemParser.h"		// Own interface
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "WorkerTask.h"					// ss::WorkerTask
#include "SamsonSetup.h"				// ss::SamsonSetup

namespace ss {

#pragma mark WorkerTaskItemParser
	
	void WorkerTaskItemParser::setupInputs()
	{
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		size_t size = FileManagerReadItem::sizeOfFile( fileName );
		
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , smi->data , task->taskManager );
		addInputFiles( item );
		
		// The delegate is TaskManager: it received notifications form the FileManager and unblock items
	}
	
	void WorkerTaskItemParser::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		Operation * op = pa->taskManager->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->taskManager->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		p.mutable_worker_task()->CopyFrom( workerTask );
		p.set_input_shm( shm_input );						// Set the input shared memory buffer
		
		if( op->getType() == Operation::parserOut)
		{
			p.set_output_txt(true);
			p.set_output_kvs(false);
		}
		else
		{
			p.set_output_txt(false);
			p.set_output_kvs(true);
		}
		
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;
	}	
	
#pragma mark WorkerTaskItemOrganizer
	
	
	WorkerTaskItemOrganizer::WorkerTaskItemOrganizer( const network::WorkerTask & _workerTask ) : WorkerTaskItem( _workerTask )
	{
		// It is no necessary to have a shared memory area
		state = ready_to_load_inputs;
	}
	
	
	void WorkerTaskItemOrganizer::setupInputs()
	{
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( workerTask );
		
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
			addInputFiles( getFileMangerReadItem( task->reduceInformation->file[f] ) );
	}
	
	void WorkerTaskItemOrganizer::run( ProcessAssistant *pa )
	{
		
		// Organize the reduce in multiple WorkerTaskItems to process each set of hash-groups
				
		int num_process = SamsonSetup::shared()->num_processes;
		
		int max_num_hgs = KV_NUM_HASHGROUPS / num_process;	// Minimum num_process divisions for force multicore approach
		size_t max_item_size = SamsonSetup::shared()->shared_memory_size_per_buffer - max_num_hgs*sizeof(FileKVInfo) - sizeof(SharedHeader);
		
		
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		reduceInformation->setup();
		
		
		// Create necessary reduce operations
		
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->size_of_hg[0];	// Total size for this operation
		
		while( hg < KV_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->size_of_hg[hg];
			
			if( ( ( total_size + current_hg_size  ) > max_item_size ) || (hg - item_hg_begin ) > max_num_hgs )
			{
				
				if( total_size > 0 )
				   task->addItem( new WorkerTaskItemOperation( item_hg_begin , hg , this,  workerTask ) );

				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
		// Create the last item
		task->addItem( new WorkerTaskItemOperation( item_hg_begin , KV_NUM_HASHGROUPS ,this,  workerTask ) );
		
	}
	
	
#pragma mark WorkerTaskItemOperation
	
	void WorkerTaskItemOperation::setupInputs()
	{
		// Access the shared memory area
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( shm_input );
		char* data = smi->data;

		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			SharedHeader header = reduceInformation->file[f]->getSharedHeader( hg_begin , hg_end);
			memcpy(data+offset, &header, sizeof(SharedHeader));
			offset+= sizeof(SharedHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( FileKVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			FileManagerReadItem *item 
					= new FileManagerReadItem( \
							reduceInformation->file[f]->fileName , \
							reduceInformation->file[f]->getFileOffset( hg_begin ), \
							header.info.size, \
							data + offset \
							, NULL );
			addInputFiles( item );
			offset += header.info.size;
		}
		
	}
	
	void WorkerTaskItemOperation::run( ProcessAssistant *pa )
	{
		
		// Create the Framework to run the operation from the ProcessAssitant side
		std::string operation = workerTask.operation();
		
		Operation * op = pa->taskManager->worker->modulesManager.getOperation( operation );
		assert( op );
		
		network::ProcessMessage p;
		p.set_code( network::ProcessMessage::run );
		p.set_operation( operation );
		p.set_num_servers( pa->taskManager->worker->workers );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		p.set_input_shm( shm_input );						// Set the input shared memory buffer
		p.set_num_input_files( task->reduceInformation->total_num_input_files );
		p.mutable_worker_task()->CopyFrom( workerTask );
		p.set_num_hash_groups( num_hash_groups );
		p.set_hg_begin( hg_begin );
		p.set_hg_end( hg_end );

		
		if( op->getType() == Operation::parserOut)
		{
			p.set_output_txt(true);
			p.set_output_kvs(false);
		}
		else
		{
			p.set_output_txt(false);
			p.set_output_kvs(true);
		}
		
		// Create the framework here at the process assistant side
		framework = new ProcessAssistantOperationFramework(pa , p );
		
		// Blocking command until the Process has complete the job
		pa->runCommand( p );	
		
		// Flush output
		framework->flushOutput(this);
		
		delete framework;	
	}
	
	
	
	
}


#include "WorkerTaskItemOperation.h"			// Own interface
#include "WorkerTask.h"							// ss::WorkerTask
#include "FileManagerReadItem.h"				// ss::FileManagerReadItem
#include "FileManager.h"						// ss::FileManager

namespace ss
{
	
	WorkerTaskItemOperation::WorkerTaskItemOperation( int _hg_begin , int _hg_end , WorkerTaskItemOrganizer* reduceOrganizer, const network::WorkerTask &task ) : WorkerTaskItem( task )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// This state indicate that we need a shared memory region to run this item
		//state = no_memory;
	}
	
	WorkerTaskItemOperation::~WorkerTaskItemOperation()
	{
	}
	
	
	void WorkerTaskItemOperation::setupInputs()
	{
		// Access the shared memory area
		SharedMemoryItem * smi = MemoryManager::shared()->getSharedMemory( get_shm_input() );
		char* data = smi->data;
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			SharedHeader header = reduceInformation->file[f]->getSharedHeader( hg_begin , hg_end );
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
									  smi->getSimpleBufferAtOffset(offset) \
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
		p.set_num_servers( pa->taskManager->worker->network->getNumWorkers() );
		p.set_num_inputs( op->getNumInputs() );
		p.set_num_outputs( op->getNumOutputs() );
		p.set_output_shm( pa->output_shm );					// Set the output shared memory buffer
		p.set_input_shm( get_shm_input() );						// Set the input shared memory buffer
		p.set_num_input_files( task->reduceInformation->total_num_input_files );
		p.mutable_worker_task()->CopyFrom( workerTask );
		
		
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
		network::ProcessMessage answer = pa->runCommand( p );
		
		if ( answer.code() == network::ProcessMessage::crash )
			setError("Operation crashed");
		
		if ( answer.code() == network::ProcessMessage::error )
			setError("Internal error in the operation");
		
		// Flush output
		if( !error )
			framework->flushOutput(this);

		delete framework;
	}
	
	
}
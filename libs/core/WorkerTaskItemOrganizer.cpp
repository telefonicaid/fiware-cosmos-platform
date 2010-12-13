

#include "WorkerTaskItemOrganizer.h"		// Own interface
#include "WorkerTask.h"						// ss::WorkerTask
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "WorkerTaskItemOperation.h"		// ss::WorkerTaskItemOperation

namespace ss {


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
	
}
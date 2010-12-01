



#ifndef _H_WORKER_TASK_ITEM_PARSER
#define _H_WORKER_TASK_ITEM_PARSER

#include "WorkerTaskItemWithOutput.h"	// ss::WorkerTaskItemWithOutput
#include "ProcessAssistant.h"			// ss::ProcessAssistant
#include "SamsonWorker.h"				// ss::SamsonWorker
#include "ProcessAssistantOperationFramework.h"	// ss::ProcessAssistantOperationFramework

namespace ss {
	
	// Individual item of a parser

	class ProcessAssistant;
	
	class WorkerTaskItemOperation : public WorkerTaskItemWithOutput
	{
		
	public:
		
		std::string operation;		// operation name
		std::string fileName;		// Input file to parse
		ProcessAssistantOperationFramework * framework;
		
		WorkerTaskItemOperation(  std::string _fileName , const network::WorkerTask &task ) : WorkerTaskItemWithOutput( task )
		{
			operation = task.operation();
			fileName = _fileName;

			// This state indicate that we need a shared memory region to run this item
			state = no_memory;

		}
	
		void setupInputs();
		
		void run( ProcessAssistant *pa );

		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			framework->flushOutput(this);
		}
		
	};
	
	
	/**
	 Item necessary to organize the reduce operation
	 */
	
	class WorkerTaskItemReduceOrganizer : public WorkerTaskItem
	{
		
		network::WorkerTask workerTask;

		// Space for the pointer info
		FileKVInfo ***info;
		
		int num_inputs;
		int *num_input_files;
		int total_num_input_files;

		WorkerTask * parentTask;	// Pointer to parent task to add children items when necessary
	
		friend class WorkerTaskItemReduce;
		
	public:
		 
		WorkerTaskItemReduceOrganizer( WorkerTask * _parentTask, const network::WorkerTask & _workerTask )
		{
			parentTask = _parentTask;	// Keep a pointer to the parent task to add new items
			
			workerTask = _workerTask;	// Copy all the information contained in the message
			
			num_inputs = workerTask.input_size();
			
			num_input_files = (int*) malloc( sizeof(int) * num_inputs );
			
			info =  (FileKVInfo***) malloc( sizeof( FileKVInfo**) * num_inputs );	// Number of inputs
			
			for( int i = 0 ; i < num_inputs ; i++)
			{
				num_input_files[i] = workerTask.input(i).file_size();
				
				//std::cout << "Num files for input " << i << " -> " << num_input_files[i] << std::endl;
				
				info[i] = (FileKVInfo **) malloc( sizeof(FileKVInfo *) * num_input_files[i] );
				for (int f = 0; f < num_input_files[i] ; f++)
					info[i][f] = (FileKVInfo*) malloc( KV_HASH_GROUP_VECTOR_SIZE_FILE );
			}
			
			// It is no necessary to have a shared memory area
			state = ready_to_load_inputs;
			
		}
		
		~WorkerTaskItemReduceOrganizer()
		{
			// Free everything
			for (int i = 0 ; i < num_inputs ; i++)
			{
				for (int f = 0; f < num_input_files[i] ; f++)
					free( info[i][f] );
				free( info[i] );
			}
			free( info );
		}
		
		
		void setupInputs();

		void run( ProcessAssistant *pa );

		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			// Nothing good if this is called
			assert( false );
		}
		
		
	};
	
	
	/**
	 Reduce opertion itself
	 */


	class WorkerTaskItemReduce : public WorkerTaskItemWithOutput
	{
		
	public:
		
		ProcessAssistantOperationFramework * framework;
		network::WorkerTask workerTask;
		
		int hg_begin;
		int hg_end;
		
		char *reduce_header;

		// Pointers inside the header
		int *header_num_hash_groups;
		int *header_num_inputs;
		int *header_num_inputs_files;
		
		WorkerTaskItemReduce( int _hg_begin , int _hg_end , WorkerTaskItemReduceOrganizer* reduceOrganizer, const network::WorkerTask &task ) : WorkerTaskItemWithOutput( task )
		{
			// Save all the information
			workerTask = task;

			// Starting and ending hash-group
			hg_begin = _hg_begin;
			hg_end = _hg_end;
			
			// Prepare the header for this item
			//
			// num_hash_groups
			// num_inputs
			// for each input--> num files
			// for each file --> HG Info
			
			int num_hash_groups = hg_end - hg_begin;
			
			reduce_header = (char *) malloc( sizeof(int) + sizeof(int) + sizeof(int)*reduceOrganizer->num_inputs + sizeof(FileKVInfo)*reduceOrganizer->total_num_input_files );

			// Full this header
			
			int *header_num_hash_groups		= (int*) reduce_header;
			int *header_num_inputs			= (int*) ( reduce_header + sizeof(int) );
			int *header_num_inputs_files	= (int*) ( reduce_header + sizeof(int) + sizeof(int) ); 
			
			
			*header_num_hash_groups = num_hash_groups;
			*header_num_inputs = reduceOrganizer->num_inputs;
			for (int i = 0 ; i < reduceOrganizer->num_inputs ; i++)
				header_num_inputs_files[i] = reduceOrganizer->num_input_files[i];
			
			FileKVInfo *info = (FileKVInfo*) ( reduce_header + sizeof(int) + sizeof(int) + sizeof(int)*reduceOrganizer->num_inputs );
			
			for (int i = 0 ; i < reduceOrganizer->num_inputs ; i++ )
			{
				for (int f = 0 ; f < reduceOrganizer->num_input_files[i] ; f++)
				{

					memcpy(info,  (char*) &reduceOrganizer->info[i][f][ hg_begin+1] ,num_hash_groups * sizeof( FileKVInfo ) );
					
					info += num_hash_groups * sizeof( FileKVInfo );
					
				}
			}
			 
			
			
			// This state indicate that we need a shared memory region to run this item
			state = no_memory;
			
		}
		
		~WorkerTaskItemReduce()
		{
			free( reduce_header );
		}
		
		void setupInputs();
		void run( ProcessAssistant *pa );
		
		virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )
		{
			framework->flushOutput(this);
		}
		
	};
	
	
}

#endif


#include "WorkerSubTask.h"			// Own interface
#include "WorkerTaskManager.h"		// ss::WorkerTaskManager
#include "WorkerTask.h"				// ss::WorkerTask
#include "ProcessItem.h"			// ss::ProcessItem
#include "ProcessGenerator.h"		// ss::ProcessGenerator
#include "FileManagerReadItem.h"	// ss::FileManagerReadItem
#include "WorkerTask.h"						// ss::WorkerTask
#include "SamsonSetup.h"					// ss::SamsonSetup
#include "FileManagerReadItem.h"			// ss::FileManagerReadItem
#include "ProcessOperation.h"				// ss::ProcessOperation
#include "ProcessParser.h"					// ss::ProcessParser
#include "ProcessManager.h"					// ss::ProcessManager
namespace ss
{
	
	WorkerSubTask::WorkerSubTask( WorkerTask *_task )
	{
		task = _task;
		
		num_read_operations_confirmed = 0;

		description = "U"; // Unknown
	}

	
	
	ProcessItem *WorkerSubTask::getProcessItem()
	{
		ProcessItem *item = _getProcessItem();
		if( item )
		{
			item->setProcessManagerDelegate(task->taskManager);		// Delegate is the task manager
			item->tag = task->task_id;								// Tag is used with the number of the task
			item->sub_tag = id;										// id of the subtask
			item->component = WORKER_TASK_COMPONENT_PROCESS;		// Component to recognize the delegate call
		}
		
		return item;
	}
	
	MemoryRequest *WorkerSubTask::getMemoryRequest()
	{
		MemoryRequest*request = _getMemoryRequest();
		
		if( request )
		{
			request->tag = task->task_id;								// Tag is used with the number of the task
			request->sub_tag = id;										// id of the subtask
			request->component = WORKER_TASK_COMPONENT_PROCESS;		// Component to recognize the delegate call
			
		}
		
		return request;
		
	}
	
	
	std::vector< FileManagerReadItem*>* WorkerSubTask::getFileMangerReadItems()
	{
		std::vector< FileManagerReadItem*>* tmp = _getFileMangerReadItems();
		
		if( tmp )
		{
			
			for ( size_t i = 0 ; i < tmp->size() ; i++)
			{
				tmp->at(i)->component = WORKER_TASK_COMPONENT_PROCESS;		// Component to recognize the delegate call
				tmp->at(i)->tag = task->task_id;
				tmp->at(i)->sub_tag = id;
				tmp->at(i)->setDelegate( task->taskManager );
			}
			num_read_operations = tmp->size();
		}
		
		else
			num_read_operations = 0;
	
		num_read_operations_confirmed = 0;

         // Avoid no-files requests
		if( tmp && (tmp->size() == 0))
		{
		   delete tmp;
		   return NULL;
		}
		
		return tmp;
	}
	
	
	
	
	
#pragma mark GeneratorSubTask
	
	GeneratorSubTask::GeneratorSubTask(WorkerTask * task ) : WorkerSubTask( task  )
	{
		description = "G";
	}
	
	ProcessItem * GeneratorSubTask::_getProcessItem()
	{
		return new ProcessGenerator( task );
	}
	

#pragma mark OrganizerSubTask
	
	OrganizerSubTask::OrganizerSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );

		description = "Or"; // Organizer
		
	}
	
	// Function to get all the read operations necessary for this task
	std::vector< FileManagerReadItem*>* OrganizerSubTask::_getFileMangerReadItems()
	{
		// Create a vector of files to be read for this sub-task
		std::vector< FileManagerReadItem*>* files = new std::vector< FileManagerReadItem*>();		
		
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			FileManagerReadItem *item = getFileMangerReadItem( task->reduceInformation->file[f] );
			files->push_back( item );
		}
		
		return files;
	}
	
	// Function to get the ProcessManagerItem to run
	
	ProcessItem * OrganizerSubTask::_getProcessItem()
	{
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the reduce in multiple WorkerTaskItems to process each set of hash-groups
		// Division is done in such a way that any of the parts do not get overloaded

		// Number of paralel processes in this system
		int num_process = SamsonSetup::shared()->num_processes;

		// Maximum size per hash-group
		size_t max_size_per_group = reduceInformation->total_size / num_process;
		
		// Another limit for memory reasons
		size_t max_size_per_group2 = (SamsonSetup::shared()->memory/4) - reduceInformation->total_num_input_files*KVFILE_NUM_HASHGROUPS*sizeof(KVInfo) - sizeof(KVHeader);
		if( max_size_per_group2 < max_size_per_group)
			max_size_per_group = max_size_per_group2;
		
		size_t limit_size_per_group = MemoryManager::shared()->getMemoryInput();
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->size_of_hg[0];	// Total size for this operation
		
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->size_of_hg[hg];
			
			if( current_hg_size > limit_size_per_group )
			{
				task->setError("Max size for a hash-group exedeed. Operation not suported");
				return NULL;
			}
			
			if( ( ( total_size + current_hg_size  ) > max_size_per_group ) )
			{
				
				 if( total_size > 0 )
					 task->addSubTask( new OperationSubTask( task , item_hg_begin , hg  ) );
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}

		
		task->addSubTask( new OperationSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS ) );
		
		// No real process for this sub-task
		return NULL;
	}
	
	FileManagerReadItem * OrganizerSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		FileManagerReadItem *item = new FileManagerReadItem( file->fileName , offset , size , file->getSimpleBufferForInfo() , task->taskManager );
		item->tag = task->task_id;
		return item;
	}	
	
	
#pragma mark OperationSubTask
	 	 
	OperationSubTask::OperationSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files*( ( num_hash_groups )*sizeof(KVInfo) + sizeof(KVHeader) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
			memory_requested += task->reduceInformation->size_of_hg[hg];

		description = "Op";// Operation
	}
	
	OperationSubTask::~OperationSubTask()
	{
		if( buffer )
			MemoryManager::shared()->destroyBuffer( buffer );
	}
	
	
	
	MemoryRequest *OperationSubTask::_getMemoryRequest()
	{
		return new MemoryRequest( memory_requested, &buffer , task->taskManager);
	}
	
	
	std::vector< FileManagerReadItem*>* OperationSubTask::_getFileMangerReadItems()
	{
		 std::vector< FileManagerReadItem*>* files = new std::vector< FileManagerReadItem*>();

		// Pointer to the buffer
		char* data = buffer->getData();
		 
		 // Reduce information is stored in the parent task ( common to all reduce task-items )
		 ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		 
		 // Offset while writing into the shared memory area
		 size_t offset = 0;		
		 
		 // Write all files at the shared memory
		 for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		 {
			 // Header
			 KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			 memcpy(data+offset, &header, sizeof(KVHeader));
			 offset+= sizeof(KVHeader);
			 
			 // Copy the info vector
			 size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			 memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			 offset += size_info;
			 
			 // Schedule the read operation into the FileManager to read data content
			 FileManagerReadItem *item 
			 = new FileManagerReadItem( \
			 reduceInformation->file[f]->fileName , \
			 reduceInformation->file[f]->getFileOffset( hg_begin ), \
			 header.info.size, \
			 buffer->getSimpleBufferAtOffset(offset) \
			 , NULL );
			 
			 files->push_back( item );
			 
			 offset += header.info.size;
		 }
		 
		
		return files;
	 
	 }
	 
	// Function to get the ProcessManagerItem to run
	ProcessItem * OperationSubTask::_getProcessItem()
	{
		return new ProcessOperation( this );
	}
	
	
#pragma mark  ParserSubTask
	
	
	ParserSubTask::ParserSubTask( WorkerTask * task ,  std::string _fileName  ) : WorkerSubTask( task  )
	{
		// Filename to parse
		fileName = _fileName;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		fileSize = au::Format::sizeOfFile( SamsonSetup::shared()->dataDirectory + "/" + fileName );	

		description = "P"; // Parser
	}
	
	ParserSubTask::~ParserSubTask()
	{
		if( buffer )
			MemoryManager::shared()->destroyBuffer( buffer );
	}
	
	
	
	MemoryRequest *ParserSubTask::_getMemoryRequest()
	{
		return new MemoryRequest( fileSize, &buffer , task->taskManager);
	}
	
	
	std::vector< FileManagerReadItem*>* ParserSubTask::_getFileMangerReadItems()
	{
		std::vector< FileManagerReadItem*>* files = new std::vector< FileManagerReadItem*>();
		
		// Single file to be parsed
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0, fileSize, buffer->getSimpleBuffer() , NULL );
		
		files->push_back( item );
		
		return files;
		
	}
	
	// Function to get the ProcessManagerItem to run
	ProcessItem * ParserSubTask::_getProcessItem()
	{
		return new ProcessParser( this );
	}
	
#pragma mark SystemSubTask
	
	SystemSubTask::SystemSubTask( WorkerTask *_task  ) : WorkerSubTask( _task )
	{
		// Create the reduce information ( stored at the worker task to share with the rest of reduce items )
		task->reduceInformation = new ProcessAssistantSharedFileCollection( task->workerTask );
		description = "Sys"; // System

		// add all the File elements to be removed when the operation is complete
		for (int i = 0 ; i < task->workerTask.input_queue(0).file_size() ; i++)
		{
			network::QueueFile qf;
			qf.set_queue( task->workerTask.input_queue(0).queue().name() );
			qf.mutable_file()->CopyFrom( task->workerTask.input_queue(0).file(i) );
			task->removeFile( qf );
		}
	}
	
	// Function to get all the read operations necessary for this task
	std::vector< FileManagerReadItem*>* SystemSubTask::_getFileMangerReadItems()
	{
		// Create a vector of files to be read for this sub-task
		std::vector< FileManagerReadItem*>* files = new std::vector< FileManagerReadItem*>();		
		
		for (int f = 0 ; f < task->reduceInformation->total_num_input_files ; f++)
		{
			FileManagerReadItem *item = getFileMangerReadItem( task->reduceInformation->file[f] );
			files->push_back( item );
		}
		
		return files;
	}
	
	// Function to get the ProcessManagerItem to run
	
	ProcessItem * SystemSubTask::_getProcessItem()
	{
		
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		reduceInformation->setup();
		
		// Organize the content so, that we generate necessary files
		size_t max_item_content_size = SamsonSetup::shared()->max_file_size;
		
		// Create necessary reduce operations
		int hg = 1;												// Evaluating current hash group	
		int	item_hg_begin = 0;									// Starting at hash-group
		size_t total_size = reduceInformation->size_of_hg[0];	// Total size for this operation
		
		while( hg < KVFILE_NUM_HASHGROUPS )
		{
			size_t current_hg_size = reduceInformation->size_of_hg[hg];
			
			if( ( ( total_size + current_hg_size  ) > max_item_content_size ) )
			{
				
				if( total_size > 0 )
					task->addSubTask( new CompactSubTask( task , item_hg_begin , hg  ) );
				
				
				// Ready for the next item
				item_hg_begin = hg;
				total_size = current_hg_size;
			}
			else
				total_size+=current_hg_size;
			
			hg++;
		}
		
		task->addSubTask( new CompactSubTask( task ,item_hg_begin , KVFILE_NUM_HASHGROUPS ) );
		 
		// No real process for this sub-task
		return NULL;
	}
	
	FileManagerReadItem * SystemSubTask::getFileMangerReadItem( ProcessAssistantSharedFile* file  )
	{
		// Read the key-value information for each hash group for each input files
		size_t offset			= sizeof( KVHeader );					// We skip the file header
		size_t size				= sizeof(KVInfo) * KVFILE_NUM_HASHGROUPS;
		
		FileManagerReadItem *item = new FileManagerReadItem( file->fileName , offset , size , file->getSimpleBufferForInfo() , task->taskManager );
		item->tag = task->task_id;
		return item;
	}		
	
#pragma mark -
	
	
	CompactSubTask::CompactSubTask( WorkerTask * task , int _hg_begin , int _hg_end  ) : WorkerSubTask( task  )
	{
		// Starting and ending hash-group
		hg_begin = _hg_begin;
		hg_end = _hg_end;
		num_hash_groups = hg_end - hg_begin;
		
		// It will be assigned calling a MemoryRequest
		buffer = NULL;
		
		// Compute the required size for this operation
		memory_requested = task->reduceInformation->total_num_input_files * ( sizeof(KVHeader) + ( num_hash_groups )*sizeof(KVInfo) );
		for (int hg = hg_begin ; hg < hg_end ; hg++)
			memory_requested += task->reduceInformation->size_of_hg[hg];
		
		description = "Sys";// System
	}
	
	CompactSubTask::~CompactSubTask()
	{
		if( buffer )
			MemoryManager::shared()->destroyBuffer( buffer );
	}
	
	
	
	MemoryRequest *CompactSubTask::_getMemoryRequest()
	{
		return new MemoryRequest( memory_requested, &buffer , task->taskManager);
	}
	
	
	std::vector< FileManagerReadItem*>* CompactSubTask::_getFileMangerReadItems()
	{
		std::vector< FileManagerReadItem*>* files = new std::vector< FileManagerReadItem*>();
		
		// Pointer to the buffer
		char* data = buffer->getData();
		
		// Reduce information is stored in the parent task ( common to all reduce task-items )
		ProcessAssistantSharedFileCollection *reduceInformation = task->reduceInformation;
		
		// Offset while writing into the shared memory area
		size_t offset = 0;		
		
		// Write all files at the shared memory
		for (int f = 0 ;  f < reduceInformation->total_num_input_files ; f++)
		{
			// Header
			KVHeader header = reduceInformation->file[f]->getKVHeader( hg_begin , hg_end );
			memcpy(data+offset, &header, sizeof(KVHeader));
			offset+= sizeof(KVHeader);
			
			// Copy the info vector
			size_t size_info = ( hg_end - hg_begin ) * sizeof( KVInfo );
			memcpy(data + offset , &task->reduceInformation->file[f]->info[hg_begin],  size_info  );
			offset += size_info;
			
			// Schedule the read operation into the FileManager to read data content
			FileManagerReadItem *item 
			= new FileManagerReadItem( \
									  reduceInformation->file[f]->fileName , \
									  reduceInformation->file[f]->getFileOffset( hg_begin ), \
									  header.info.size, \
									  buffer->getSimpleBufferAtOffset(offset) \
									  , NULL );
			
			files->push_back( item );
			
			offset += header.info.size;
		}
		
		
		return files;
		
	}
	
	// Function to get the ProcessManagerItem to run
	ProcessItem * CompactSubTask::_getProcessItem()
	{
		return new ProcessCompact( this );
	}
	
	
}

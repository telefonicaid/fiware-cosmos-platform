#ifndef _H_WORKER_SUB_TASK
#define _H_WORKER_SUB_TASK

#include <cstring>			// size_t
#include <vector>			// std::vector
#include <string>			// std::string

namespace ss
{
	
	class WorkerTask;
	class MemoryRequest;
	class FileManagerReadItem;
	class ProcessItem;
	class ProcessAssistantSharedFile;
	class Buffer;
	
	/**
	 Subtask inside WorkTask.
	 It could be a generator , map , reduce , organizer , parser , etc...
	 */
	
	class WorkerSubTask
	{
		int num_read_operations;
		int num_read_operations_confirmed;
		
	public:
		
		WorkerTask *task;
		size_t id;
		
		WorkerSubTask( WorkerTask *_task );
		virtual ~WorkerSubTask(){};		// Destructor has to be virtual ( calling delete from this parent class )
		
		// Function to get the memory request ( if any )
		virtual MemoryRequest *_getMemoryRequest()
		{
			return NULL;
		}
		
		// Function to get all the read operations necessary for this task
		virtual std::vector< FileManagerReadItem*>* _getFileMangerReadItems()
		{
			return NULL;
		}
		
		// Function to get the ProcessManagerItem to run
		virtual ProcessItem *_getProcessItem()
		{
			return NULL;
		}

		// Function to get all the read operations necessary for this task
		std::vector< FileManagerReadItem*>* getFileMangerReadItems();
		
		// Function to get the ProcessManagerItem to run
		ProcessItem *getProcessItem();
		
		// Function to get the memory request ( if any )
		MemoryRequest *getMemoryRequest();
		
		
		bool notifyReadFinish()
		{
			num_read_operations_confirmed++;
			return( num_read_operations_confirmed == num_read_operations);
		}
		
	};
	
	
	// Unique sub task for generator
	
	class GeneratorSubTask : public WorkerSubTask
	{
		
	public:
		
		GeneratorSubTask( WorkerTask * task  ) : WorkerSubTask( task  )
		{
		}
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();
		
	};

	
	// SubTask to organize any map, reduce, parserOut operations
	
	class OrganizerSubTask : public WorkerSubTask
	{		
	public:
		
		OrganizerSubTask( WorkerTask *task);		
		
		
		// Function to get all the read operations necessary for this task
		std::vector< FileManagerReadItem*>* _getFileMangerReadItems();
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();		
		
	private:
		
		FileManagerReadItem * getFileMangerReadItem( ProcessAssistantSharedFile* file );
		
	};	
	
	
	// Subtask that runs a particular sub-tasks like a map,reduce or parserOut
		 
	 class OperationSubTask : public WorkerSubTask
	 {
	 
	 public:

		 int hg_begin;
		 int hg_end;
		 int num_hash_groups;
		 
		 size_t memory_requested;
		 Buffer *buffer;				// Buffer provided by memory request

		 OperationSubTask( WorkerTask * task , int _hg_begin , int _hg_end  );
		 
		 ~OperationSubTask();
		 
		 MemoryRequest *_getMemoryRequest();
		 
		 // Function to get all the read operations necessary for this task
		 std::vector< FileManagerReadItem*>* _getFileMangerReadItems();

		 // Function to get the ProcessManagerItem to run
		 ProcessItem * _getProcessItem();		
	 
	 };	
	
	
	// Subtask that runs a particular sub-tasks like a map,reduce or parserOut
	
	class ParserSubTask : public WorkerSubTask
	{
		
	public:
		
		std::string fileName;		// Input file name
		size_t fileSize;			// Memory requested for the input file
		
		Buffer *buffer;				// Buffer provided by memory request
		
		ParserSubTask( WorkerTask * task , std::string fileName  );
		
		~ParserSubTask();
		
		MemoryRequest *_getMemoryRequest();
		
		// Function to get all the read operations necessary for this task
		std::vector< FileManagerReadItem*>* _getFileMangerReadItems();
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();		
		
	};	
		
	

}

#endif
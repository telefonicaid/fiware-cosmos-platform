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
	 ------------------------------------------------------------------------------------------------
	 WorkerSubTask is a subclass describing a particular operation executed inside a WorkerTask
		It could be a generator , map , reduce , organizer , parser , etc...
		All the sub-tasks is the SAMSON ecosystem are divided in three steps:	
			- Memory request
			- Read operations
			- Process item
	 
		Rigth now, these three steps are executed sequentially. Three function return the necessitied 
		for each particular subtask.
	 ------------------------------------------------------------------------------------------------
	 */
	
	class WorkerSubTask
	{
		int num_read_operations;
		int num_read_operations_confirmed;

	public:

		std::string description;	// Short description for debuggin
				
		WorkerTask *task;			// Pointer to the parent task
		size_t id;					// ID of the parent task
		
		// Constructor with the parent task
		WorkerSubTask( WorkerTask *_task );
		
		// Destructor has to be virtual ( calling delete from this parent class )
		virtual ~WorkerSubTask(){};		
		
		// Function to get the memory request ( if any )
		virtual MemoryRequest *_getMemoryRequest()
		{
			return NULL;
		}
		
		// Function to get all the read operations necessary for this task ( if any )
		virtual std::vector< FileManagerReadItem*>* _getFileMangerReadItems()
		{
			return NULL;
		}
		
		// Function to get the ProcessManagerItem to run ( if necessary )
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
	
	/*
	------------------------------------------------------------------------------------------------
	 GeneratorSubTask
	 
	 Specific sub-tasks for generator tasks

	------------------------------------------------------------------------------------------------
	*/
	
	class GeneratorSubTask : public WorkerSubTask
	{
		
	public:
		
		GeneratorSubTask( WorkerTask * task  );
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();
	};
	
	/*
	 ------------------------------------------------------------------------------------------------
	 OrganizerSubTask
	 
		Tasks that organize the operationsSubTasks necessary to process a particular data set
		It reads the "info" vectors for ech involved file and decides how to organize
		groups of hash-groups fo efficiency
	 ------------------------------------------------------------------------------------------------
	 */
		
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

	
	/*
	 ------------------------------------------------------------------------------------------------
	 OperationSubTask
	 
		Basic task for a map, reduce, parseOut
	 
	 ------------------------------------------------------------------------------------------------
	 */
		 
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

	
	
	/*
	 ------------------------------------------------------------------------------------------------
	 ParserSubTask
	 
	 Specific sub-tasks for parser tasks
	 
	 ------------------------------------------------------------------------------------------------
	 */
		
	
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
		
	
	/*
	 ------------------------------------------------------------------------------------------------
	 ParserSubTask
	 
	 Specific sub-tasks system operations ( like compact )
	 
	 ------------------------------------------------------------------------------------------------
	 */
		
	class SystemSubTask : public WorkerSubTask
	{		
	public:
		
		SystemSubTask( WorkerTask *task );		
		
		// Function to get all the read operations necessary for this task
		std::vector< FileManagerReadItem*>* _getFileMangerReadItems();
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();		
		
	private:
		
		FileManagerReadItem * getFileMangerReadItem( ProcessAssistantSharedFile* file );
	};	

	/**
	 Spetial operation to compact files into new set of files
	 */
	
	class CompactSubTask : public WorkerSubTask
	{
		
	public:
		
		int hg_begin;
		int hg_end;
		int num_hash_groups;
		
		size_t memory_requested;
		
		Buffer *buffer;						// Buffer provided by memory request
		
		CompactSubTask( WorkerTask * task , int _hg_begin , int _hg_end  );
		
		~CompactSubTask();

		// Memory request for this task
		MemoryRequest *_getMemoryRequest();
		
		// Function to get all the read operations necessary for this task
		std::vector< FileManagerReadItem*>* _getFileMangerReadItems();
		
		// Function to get the ProcessManagerItem to run
		ProcessItem * _getProcessItem();		
		
	};	
	
}

#endif


#ifndef _H_PROCESS_BASE
#define _H_PROCESS_BASE


#include <cstring>				// size_t		
#include <assert.h>				// assert(.)
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "FileManager.h"		// ss::FileManager ( and derivates )
#include "samson.pb.h"			// ss::network::...
#include "coding.h"				// ss::ProcessAssistantSharedFile
#include "Lock.h"				// au::Lock
#include "ProcessItem.h"		// ss::ProcessItem
#include "ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "NetworkInterface.h"		// ss::NetworkInterface
#include "ProcessWriter.h"			// ss::ProcessWriter
#include "samson.pb.h"				// ss::network::...
#include "samson/OperationController.h"	// ss::OperationController

#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER	1


namespace ss
{
	
	class WorkerTask;
	
	/**
	 A particular process that runs in as isolated mode generating key-values
	 A shared memory area is used to exchange data between the background process and the foreground thread
	 When necessary, a code is sent between both to flush content of this shared memory segment
	 */
	
	class ProcessBase : public ProcessItemIsolated
	{

	public:
		
		typedef enum
		{
			key_value,
			txt
		} ProcessBaseType;
		
		
		ProcessBaseType type;
		
		// Elements defining this task
		//WorkerTask *task;
		
		int num_outputs;						// Number of outputs
		int num_servers;						// Number of workers in the cluster

		size_t task_id;
		
		network::WorkerTask *workerTask;		// Message received from the controller
		//WorkerTaskManager *workerTaskManager;	// Pointer to the task manager
		NetworkInterface *network;
		
		int shm_id;							// Shared memory area used in this operation
		SharedMemoryItem *item;				// Share memory item
		
		Environment environment;			// Environment of the operation
		
	public:
		
		ProcessBase( WorkerTask *task , ProcessBaseType type );
		~ProcessBase();
		
		
		void setProcessBaseMode(ProcessBaseType _type)
		{
			type = _type;
		}
		
		void init()
		{
			shm_id = MemoryManager::shared()->retainSharedMemoryArea();
			item = MemoryManager::shared()->getSharedMemory( shm_id );
		}
		
		void finish()
		{
			MemoryManager::shared()->releaseSharedMemoryArea( shm_id );		
			MemoryManager::shared()->freeSharedMemory( item );
		}
		
		// Function to be implemented ( running on a different process )
		void runIsolated();
		void runIsolatedKV();
		void runIsolatedTXT();
		
		// Function to generate the key-values with the Writer element ( back process )
		virtual void generateKeyValues( KVWriter *writer ){};

		// Function to generate the txt buffers at the output ( back process )
		virtual void generateTXT( TXTWriter *writer ){};
		
		
		// Flush the buffer ( front process ) in key-value and txt mode
		void flushBuffer( );
		void flushKVBuffer( );
		void flushTXTBuffer( );
		
		// Function executed at this process side when a code is sent from the background process
		void runCode( int c )
		{
			switch (c) {
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
					flushBuffer();	// Flush the generated buffer with new key-values
					break;
				default:
					setError("System error: Unknown code in the isolated process communication");
					break;
			}
		}
		/*
		 
		 public:
		 
		 enum State
		 {
		 definition,				// Defining the task
		 no_memory,				// Waiting for a shared memory item to be assigned
		 ready_to_load_inputs,	// Ready to load data from inputs
		 loading_inputs,			// Loading data from disk
		 ready_to_run,			// Ready to be process by any ProcessAssistant
		 running					// It has beent taken by a ProcessAssistant (should report finish)
		 };
		 
		 
		 private:
		 
		 int shm_input;				// Shared memory identifier for input ( if necessary ) -1 --> no assigned
		 State state;				// Main state of this item
		 
		 public:
		 
		 WorkerTask *task;			// Pointer to the parent task
		 
		 int item_id;				// Item id ( from 0 to ...)
		 
		 au::Lock lock;				// Lock to protect num_input_files and confirmed_input_files
		 int num_input_files;		// Number of input files requested for this item during loading phase	
		 int confirmed_input_files;	// Number of files confirmed by FileManager
		 
		 
		 
		 
		 // Set the parent task
		 void setTaskAndItemId( WorkerTask *task , int itemId);
		 
		 
		 // Function to define the required input files for this item ( this depends on the concrete type of item)
		 void addInputFiles( FileManagerReadItem *item );	// Function used inside setupInputs in subclasses
		 
		 // Call back from the File Manager to confirm that an input file is ready
		 void fileManagerNotifyFinish(size_t id, bool success);
		 
		 
		 // General setup while asking if it is ready
		 
		 bool isReadyToRun();
		 
		 void setError( std::string _error_message );
		 
		 void start();
		 
		 void setup();
		 
		 void freeResources();
		 
		 State getState();
		 
		 // Funciton to get the run-time status of this object
		 void getStatus( std::ostream &output , std::string prefix_per_line );
		 
		 std::string getStatus();
		 
		 // Particular operations to run
		 virtual void run(ProcessAssistant *pa)=0;
		 virtual void receiveCommand( ProcessAssistant *pa , network::ProcessMessage p )=0;
		 
		 
		 // Utility function to get the o-th output queue
		 network::Queue getOutputQueue(int o);
		 
		 int get_shm_input()
		 {
		 return shm_input;
		 }
		 
		 static const char * getStateDescription( State s )
		 {
		 switch (s) {
		 case definition:	return "Definition";
		 case no_memory:		return "Memory";
		 case ready_to_load_inputs: return "Ready to load";
		 case loading_inputs: return "Loading";
		 case ready_to_run:	return "ready_to_run";
		 case running:		return "running";
		 }
		 return "unknown";
		 
		 }
		 
		 protected:
		 
		 FileManagerReadItem * getFileMangerReadItem( ProcessAssistantSharedFile* file  );
		 
		 virtual void setupInputs(){}	
		 */
		
	};
}

#endif
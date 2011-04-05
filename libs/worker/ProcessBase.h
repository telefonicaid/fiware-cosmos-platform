
#ifndef _H_PROCESS_BASE
#define _H_PROCESS_BASE


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "samson.pb.h"			// ss::network::...
#include "coding.h"				// ss::ProcessAssistantSharedFile
#include "Lock.h"				// au::Lock
#include "ProcessItem.h"		// ss::ProcessItem
#include "ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "NetworkInterface.h"		// ss::NetworkInterface
#include "ProcessWriter.h"			// ss::ProcessWriter
#include "samson.pb.h"                      // ss::network::...
#include "samson/OperationController.h"     // ss::OperationController
#include "Engine.h"                         // ss::Engine
#include "SharedMemoryManager.h"            // ss::SharedMemoryManager


#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER          1
#define WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH	2


namespace ss
{
	
	class WorkerTask;
	class WorkerTaskManager;
	class SharedMemoryItem;
    
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
        int worker;                             // Information about the my worker id
		int hg_set;                             // Inormation about the hash-group of this process
        
		network::WorkerTask *workerTask;		// Message received from the controller
		WorkerTaskManager *workerTaskManager;	// Pointer to the task manager
		NetworkInterface *network;
		
		
		Environment environment;			// Environment of the operation
		
	public:
		
		ProcessBase( WorkerTask *task , ProcessBaseType type );
		~ProcessBase();
		
		
		void setProcessBaseMode(ProcessBaseType _type)
		{
			type = _type;
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
		int flushBuffer( bool finish );
		int flushKVBuffer( bool finish );
		int flushTXTBuffer( bool finish );
		
		// Function executed at this process side when a code is sent from the background process
		int runCode( int c )
		{
			switch (c) {
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
					return flushBuffer(false);	// Flush the generated buffer with new key-values
					break;
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
					return flushBuffer(true);	// Flush the generated buffer with new key-values
					break;
				default:
					error.set("System error: Unknown code in the isolated process communication");
					break;
			}
			
			return PI_CODE_KILL;
		}

		
	};
}

#endif
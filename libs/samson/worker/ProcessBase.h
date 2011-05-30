
#ifndef _H_PROCESS_BASE
#define _H_PROCESS_BASE


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream
#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// samson::ProcessAssistantSharedFile
#include "au/Lock.h"				// au::Lock
#include "engine/ProcessItem.h"		// samson::ProcessItem
#include "samson/isolated/ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "samson/network/NetworkInterface.h"		// samson::NetworkInterface
#include "samson/isolated/ProcessWriter.h"			// samson::ProcessWriter
#include "samson/common/samson.pb.h"                      // samson::network::...
#include "samson/module/OperationController.h"     // samson::OperationController
#include "engine/Engine.h"                         // samson::Engine
#include "samson/isolated/SharedMemoryManager.h"            // samson::SharedMemoryManager
#include "samson/common/MemoryTags.h"                       // MemoryOutputNetwork
#include "samson/isolated/ProcessIsolated.h"                // samson::ProcessIsolated


namespace samson
{
	
	class WorkerTask;
	class WorkerTaskManager;
	class SharedMemoryItem;
    class ProcessWriter;
    class ProcessTXTWriter;
    
 
	/**
	 A particular process that runs in as isolated mode generating key-values
	 A shared memory area is used to exchange data between the background process and the foreground thread
	 When necessary, a code is sent between both to flush content of this shared memory segment
	 */
	
	class ProcessBase : public ProcessIsolated
	{

	public:
		
		typedef enum
		{
			key_value,
			txt
		} ProcessBaseType;
		
		
		ProcessBaseType type;
		

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
		void flushBuffer( bool finish );
		void flushKVBuffer( bool finish );
		void flushTXTBuffer( bool finish );
		
		// Function executed at this process side when a code is sent from the background process
		void runCode( int c )
		{
			switch (c) {
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER:
					flushBuffer(false);	// Flush the generated buffer with new key-values
					return;
                    break;
				case WORKER_TASK_ITEM_CODE_FLUSH_BUFFER_FINISH:
					flushBuffer(true);	// Flush the generated buffer with new key-values
                    return;
                    break;
				default:
					error.set("System error: Unknown code in the isolated process communication");
					break;
			}
			
			return;
		}

		
	};
}

#endif
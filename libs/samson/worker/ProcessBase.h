
#ifndef _H_PROCESS_BASE
#define _H_PROCESS_BASE


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream

#include "au/Lock.h"				// au::Lock

#include "engine/Engine.h"                         // samson::Engine
#include "engine/ProcessItem.h"		// samson::ProcessItem

#include "samson/common/samson.pb.h"			// samson::network::...
#include "samson/common/coding.h"				// samson::ProcessAssistantSharedFile
#include "samson/common/samson.pb.h"                      // samson::network::...
#include "samson/common/MemoryTags.h"                       // MemoryOutputNetwork

#include "samson/isolated/ProcessWriter.h"			// samson::ProcessWriter
#include "samson/isolated/ProcessItemIsolated.h"	// ss:ProcessItemIsolated
#include "samson/isolated/SharedMemoryManager.h"            // samson::SharedMemoryManager
#include "samson/isolated/ProcessIsolated.h"                // samson::ProcessIsolated

#include "samson/network/NetworkInterface.h"		// samson::NetworkInterface

#include "samson/module/OperationController.h"     // samson::OperationController


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
		

		size_t task_id;                         // Worker task_id
		int hg_set;                             // Inormation about the hash-group of this process
        
		network::WorkerTask *workerTask;		// Message received from the controller
		WorkerTaskManager *workerTaskManager;	// Pointer to the task manager
		NetworkInterface *network;              // Pointer to the network interface
		

    protected:
        
        int worker;                             // Information about the my worker id
        int num_workers;                        // Information about the total number of workers
        
	public:
		
		ProcessBase( WorkerTask *task , ProcessBaseType type );
		~ProcessBase();
		

        // Function to process the output of the operations
        void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish );
        void processOutputTXTBuffer( engine::Buffer *buffer , bool finish );
        
        
		
	};
}

#endif
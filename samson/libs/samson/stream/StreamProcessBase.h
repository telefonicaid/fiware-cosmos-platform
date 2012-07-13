
#ifndef _H_STREAM_PROCESS_BASE
#define _H_STREAM_PROCESS_BASE


#include <cstring>				// size_t		
#include <string>				// std::string
#include <sstream>				// std::ostringstream

#include "engine/ProcessItem.h"                         // samson::ProcessItem
#include "engine/Engine.h"                              // samson::Engine

#include "samson/common/samson.pb.h"                    // samson::network::...
#include "samson/common/coding.h"                       
#include "samson/common/samson.pb.h"                    // samson::network::...
#include "samson/common/MemoryTags.h"                   // MemoryOutputNetwork

#include "samson/module/OperationController.h"          // samson::OperationController

#include "samson/isolated/ProcessWriter.h"              // samson::ProcessWriter
#include "samson/isolated/ProcessItemIsolated.h"        // ss:ProcessItemIsolated
#include "samson/isolated/SharedMemoryManager.h"        // samson::SharedMemoryManager
#include "samson/isolated/ProcessIsolated.h"            // samson::ProcessIsolated

#include "samson/network/NetworkInterface.h"            // samson::NetworkInterface

#include "samson/stream/Queue.h"                        // samson::Stream::Queue

namespace samson
{
    
    namespace stream
    {
        
        class WorkerTask;
        class WorkerTaskManager;
        class SharedMemoryItem;
        class ProcessWriter;
        class ProcessTXTWriter;
        class StreamOperation;
        class StreamOperationBase;
        
        /**
         A particular process that runs in as isolated mode generating key-values
         A shared memory area is used to exchange data between the background process and the foreground thread
         When necessary, a code is sent between both to flush content of this shared memory segment
         */
        
        class StreamProcessBase : public ::samson::ProcessIsolated
        {
            
        public:
            
            size_t task_id;                         // Id of the operation
            size_t task_order;                      // Order of the task
                        
            // Information about how to process this queue
            StreamOperationBase *streamOperation;
            
        public:
            
            StreamProcessBase( size_t _task_id , StreamOperationBase* _streamOperation );
            virtual ~StreamProcessBase();
            
            // Function to process the output of the operations
            virtual void processOutputBuffer( engine::Buffer *buffer , int output , int outputWorker , bool finish );
            virtual void processOutputTXTBuffer( engine::Buffer *buffer , bool finish );
            
        protected:
            
            void sendBufferToQueue( engine::Buffer *buffer , int outputWorker , std::string queue_name );

            
        };
        
    }
    
}

#endif
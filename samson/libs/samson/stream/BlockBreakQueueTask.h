

#ifndef _H_STREAM_BLOCK_BREAK_TASK
#define _H_STREAM_BLOCK_BREAK_TASK

#include <sstream>
#include <string>

#include "au/containers/map.h"                             // au::map
#include "au/string.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Buffer.h"                      // engine::Buffer
#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/KVInfo.h"
#include "samson/common/KVFile.h"
#include "samson/common/samson.pb.h"            // network::...
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager
#include "samson/stream/SystemQueueTask.h"      // samson::stream::SystemQueueTask
#include "engine/Object.h"                      // engine::Object

namespace samson {
    
    namespace stream
    {
        /**
         Task to break a set of blocks
         */
        
        class BlockBreakQueueTask : public SystemQueueTask
        {
            friend class QueueTaskManager;
            friend class StreamManager;
            
            std::vector<engine::Buffer*> outputBuffers;     // Output buffers generated by this operation ( retained )
            std::string queue_name;                         // Name of the queue we are working on
            
            size_t output_operation_size;                   // Size of the generated buffers
            
        public:
            
            BlockBreakQueueTask( size_t _id , std::string queue_name , size_t _output_operation_size );
            ~BlockBreakQueueTask();
            
            void run();
            
            // Function executed just before task is deleted in the main thread
            void finalize( StreamManager* streamManager );
            
            void setWorkingSize()
            {
                BlockInfo block_info;
                update( block_info );
                setProcessItemWorkingSize( block_info.size );
            }
            
            
        private:

            // Vector of KVFile structures to access all blokcs
            au::vector<KVFile> files;

            // Auxiliar function to create a block
            void createBlock( int hg_begin , int hg_end );
            
        };  
    }
}

#endif
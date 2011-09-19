


#ifndef _H_STREAM_POP_QUEUE_TASK
#define _H_STREAM_POP_QUEUE_TASK

#include <sstream>
#include <string>

#include "au/map.h"                             // au::map
#include "au/string.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/coding.h"               // KVRange
#include "samson/common/samson.pb.h"            // network::...

#include "samson/stream/SystemQueueTask.h"      // samson::Stream::SystemQueueTask
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager
#include "engine/Object.h"                      // engine::Object


namespace samson {
    
    namespace stream {

        class PopQueue;
        
        class PopQueueTask : public SystemQueueTask
        {
            friend class QueueTaskManager;
            friend class StreamManager;
            
            size_t pop_queue_id;    // Id of the pop queue
            size_t delilahId;       // Identifier of the operation at delilah side
            int fromId ;            // Identifier of delilah to send packets back
            
            // Limits of operation for this pop queue
            KVRange range;
            
        public:
            
            PopQueueTask( size_t _id , PopQueue *pq , KVRange _range );
            
            void sendMessage( engine::Buffer *buffer );
            
            void run();
            
        };
    }
}

#endif


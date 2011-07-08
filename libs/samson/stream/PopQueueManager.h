

#ifndef _H_STREAM_POP_QUEUE_MANAGER
#define _H_STREAM_POP_QUEUE_MANAGER


#include <sstream>

#include "au/map.h"     // au::map
#include "au/Format.h"  // au::Format

#include <string>
#include "samson/common/samson.pb.h"        // network::...
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager
#include "engine/Object.h"            // engine::Object

namespace au
{
    class ErrorManager;
}

namespace samson {
    
    
    namespace stream
    {
        class PopQueue;
        class Queue;
        
        // Manager of all the running PopQueue operations
        class PopQueueManager
        {
            size_t id;
            au::map< size_t , PopQueue > popQueues;     // Map of current pop-queue operations
            
        public:
            
            PopQueueManager()
            {
                id = 1;    // Init the id counter
            }
            
            void add( PopQueue *pq );            
            
            std::string getStatus();
            
            void notifyFinishTask( size_t pop_queue_id , size_t task_id , au::ErrorManager *error );
                                  
        };
    }
}

#endif


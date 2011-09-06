

#ifndef _H_STREAM_POP_QUEUE_MANAGER
#define _H_STREAM_POP_QUEUE_MANAGER


#include <string>
#include <sstream>

#include "au/map.h"     // au::map
#include "au/string.h"  // au::Format

#include "engine/Object.h"            // engine::Object

#include "samson/common/samson.pb.h"        // network::...
#include "samson/stream/QueueTaskManager.h" // samson::stream::QueueTaskManager

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
        class PopQueueManager : engine::Object
        {
            size_t id;
            au::map< size_t , PopQueue > popQueues;     // Map of current pop-queue operations
            
        public:
            
            PopQueueManager();
            
            void add( PopQueue *pq );            
            
            std::string getStatus();
            
            void notifyFinishTask( size_t pop_queue_id , size_t task_id , au::ErrorManager *error );
          
            
            // Notifications
            void notify( engine::Notification* notification );

        
        private:
            
            void clearFinishPopQueues();
            
        };
    }
}

#endif


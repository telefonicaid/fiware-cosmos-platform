
#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include "au/list.h"        // au::list
#include <string>           // std::string

namespace samson {
    namespace stream {
        
        class QueueTask;
        
        class QueueTaskManager
        {
            au::list< QueueTask > queueTasks;
            size_t id;                              // Id of the current task
                        
        public:
            
            void add( QueueTask* task );
            
            std::string getStatus();
            
        };
        
    }
    
}

#endif

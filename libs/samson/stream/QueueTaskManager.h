
#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include "au/list.h"        // au::list
#include "au/map.h"         // au::map

#include "engine/Object.h"  // engine::Object

#include <string>           // std::string

namespace samson {
    namespace stream {
        
        class SystemQueueTask;
        class StreamManager;
        class PopQueueTask;
        class QueueTask;
        class SystemQueueTask;
        
        class QueueTaskManager : public ::engine::Object
        {
            size_t id;                                                      // Id of the current task

            au::list< QueueTask > queueTasks;                               // List of pending task to be executed
            au::map< size_t , QueueTask > runningTasks;                     // Map of running tasks

            au::list< SystemQueueTask > systemQueueTasks;                   // List of pending system queue tasks to be executed
            au::map< size_t , SystemQueueTask > runningSystemQueueTasks;    // Map of running system queue tasks
            
            StreamManager* streamManager;
            
        public:
            
            QueueTaskManager( StreamManager* qm );
            
            size_t getNewId();
            
            void add( QueueTask* task );
            void add( SystemQueueTask* task );
            
            void reviewPendingQueueTasks();
            bool runNextQueueTasksIfNecessary();
            bool runNextSystemQueueTasksIfNecessary();
            
            // Notifications
            void notify( engine::Notification* notification );
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
            
        };
        
    }
    
}

#endif

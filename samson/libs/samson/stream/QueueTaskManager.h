/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#ifndef _H_SAMSON_QUEUE_TASK_MANAGER
#define _H_SAMSON_QUEUE_TASK_MANAGER

#include "au/containers/list.h"        // au::list
#include "au/containers/map.h"         // au::map

#include "engine/Object.h"  // engine::Object

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"
#include "samson/common/status.h"

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
            
            void cancelForWorkerCommandId( std::string worker_command_id );
            
            void reviewPendingQueueTasks();
            bool runNextQueueTasksIfNecessary();
            bool runNextSystemQueueTasksIfNecessary();
            
            // Notifications
            void notify( engine::Notification* notification );
            
            // Get information for monitoring
            void getInfo( std::ostringstream& output);
            
            // Return is there is activity
            bool isActive();
            
            // Reset all the content of this manager
            void reset();
            
            bool hasEnougthTasks(); 

            // Get a collection for monitoring
            samson::network::Collection* getCollection( samson::VisualitzationOptions options ,  std::string pattern  );
            
        };
        
    }
    
}

#endif

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



#ifndef _H_STREAM_POP_QUEUE_TASK
#define _H_STREAM_POP_QUEUE_TASK

#include <sstream>
#include <string>

#include "au/containers/map.h"                             // au::map
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
            
            size_t delilah_id;             // Delilah identifier
            size_t delilah_component_id;   // Delilah component identifier
            
            // Limits of operation for this pop queue
            KVRange range;
            
        public:
            
            PopQueueTask( size_t _id , PopQueue *pq , KVRange _range );
            
            void sendMessage( engine::Buffer *buffer );
            
            void run();
            
        };
        
        
        class StreamOutQueueTask : public SystemQueueTask
        {
            friend class QueueTaskManager;
            friend class StreamManager;
            
            size_t fromId ;         // Identifier of delilah to send packets back
            std::string queue;      // Name of the queue we are streaming out
            
        public:
            
            StreamOutQueueTask( size_t _id , size_t _fromId, std::string _queue );
            void sendMessage( engine::Buffer *buffer );
            void run();
            
        };
        
        
    }
}

#endif


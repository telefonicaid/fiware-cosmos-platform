

#ifndef _H_STREAM_SYSTEM_QUEUE_TASK
#define _H_STREAM_SYSTEM_QUEUE_TASK

#include <sstream>
#include <string>

#include "au/map.h"                             // au::map
#include "au/string.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engibe::ProcessItem

#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/coding.h"               // KVRange
#include "samson/common/samson.pb.h"            // network::...

#include "samson/stream/QueueTaskBase.h"        // samson::stream::QueueTaskBase
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager



namespace samson {
    
    namespace stream
    {
        
        class Queue;
        class BlockList;
        class Block; 
        class StreamManager;
        
        /*
         System queue-task ( like queuTask but whithout the isolation )
         */
        
        class SystemQueueTask : public engine::ProcessItem , public QueueTaskBase
        {
            
            std::string concept;
            
        public:
            
            SystemQueueTask( size_t _id , std::string _concept);            
            ~SystemQueueTask();

            // Function executed just before task is deleted in the main thread
            virtual void finalize( StreamManager* streamManager ){};

            // xml version of the information
            void getInfo( std::ostringstream& output);
          
            
        };
    }
}
        

#endif

#ifndef _H_STREAM_POP_QUEUE
#define _H_STREAM_POP_QUEUE

#include <sstream>
#include <string>

#include "au/containers/map.h"                             // au::map
#include "au/string.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/coding.h"               // KVRange
#include "samson/common/samson.pb.h"            // network::...
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager
#include "engine/Object.h"                      // engine::Object


NAMESPACE_BEGIN(engine)
    class Buffer;
}

namespace samson {
    
    namespace stream
    {
        
        class Queue;
        class BlockList;
        class Block; 
        
        /*
         
            Class with information about a pop queue operations
            A pop queue operations allows to recover the content of a txt queue from delilah

         */

        class PopQueue : public engine::Object
        {
            friend class Queue;
            friend class PopQueueManager;
            friend class StreamManager;
            friend class PopQueueTask;
            
            size_t id;              // Id of this pop queue operation ( given by the PopQueueManager )
            
            network::PopQueue *pq;  // Message from delilah containing information about the pop operation
            
            size_t delilah_id;             // Identifier of delilah asking for this
            size_t delilah_component_id;  // Identifier of the compnent inside this delilah
            
            std::set<size_t> running_tasks;             // Tasks that are currently running
            
            bool finished;                              // Flag to identify that data generation is finished
            
        public:

            au::ErrorManager error;
            
            PopQueue( const network::PopQueue& _pq , size_t _delilah_id, size_t _delilah_component_id  );
            ~PopQueue();
            
            void addTask( size_t id );
            
            std::string getStatus();
            
            void sendFinalMessage();

            void check();

            void notifyFinishTask( size_t task_id , au::ErrorManager *_error );
            
        };
        
 
        
    }
}


#endif


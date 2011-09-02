
#ifndef _H_STREAM_POP_QUEUE
#define _H_STREAM_POP_QUEUE

#include <sstream>
#include <string>

#include "au/map.h"                             // au::map
#include "au/Format.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Object.h"                      // engine::Object
#include "engine/ProcessItem.h"                 // engine::ProcessItem


#include "samson/common/coding.h"               // KVRange
#include "samson/common/samson.pb.h"            // network::...
#include "samson/stream/QueueTaskManager.h"     // samson::stream::QueueTaskManager
#include "engine/Object.h"                      // engine::Object


namespace engine
{
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
            friend class QueuesManager;
            friend class PopQueueTask;
            
            size_t id;              // Id of this pop queue operation ( given by the PopQueueManager )
            
            network::PopQueue *pq;  // Message from delilah containing information about the pop operation
            
            size_t delilahId;       // Identifier of the operation at delilah side
            int fromId ;            // Identifier of delilah to send packets back
            
            std::set<size_t> running_tasks;             // Tasks that are currently running
            
            bool finished;                              // Flag to identify that data generation is finished
            
        public:

            au::ErrorManager error;
            
            PopQueue( const network::PopQueue& _pq , size_t _delilahId, int _fromId  );
            
            ~PopQueue();
            
            void addTask( size_t id );
            
            std::string getStatus();
            
            void sendFinalMessage();
            

            void check();

            void notifyFinishTask( size_t task_id , au::ErrorManager *_error );
            
        };
        
        
        /**
         Task in a pop queue task
         */
        
        class PopQueueTask : public engine::ProcessItem
        {
            friend class QueueTaskManager;
            friend class QueuesManager;
            
            BlockList *list;
            BlockList *list_lock;

            bool             ready_flag;
            
            size_t id;              // Id of this task ( in the QueueTaskManager )
            
            size_t pop_queue_id;    // Id of the pop queue
            size_t delilahId;       // Identifier of the operation at delilah side
            int fromId ;            // Identifier of delilah to send packets back

            // Limits of operation for this pop queue
            KVRange range;
            
        public:

            PopQueueTask( size_t _id , PopQueue *pq , KVRange _range );
            ~PopQueueTask();
            
            void addBlock( Block *b );

            void sendMessage( engine::Buffer *buffer );
            
            bool ready();
            
            void run();
            
        };
        
    }
}


#endif


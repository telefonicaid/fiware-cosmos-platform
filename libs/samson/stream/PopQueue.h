
#ifndef _H_STREAM_POP_QUEUE
#define _H_STREAM_POP_QUEUE

#include <sstream>
#include <string>

#include "au/map.h"                             // au::map
#include "au/Format.h"                          // au::Format
#include "au/ErrorManager.h"

#include "engine/Object.h"                      // engine::Object

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
        
        // Class with information about a pop queue operations
        class PopQueue : public engine::Object
        {
            friend class Queue;
            friend class PopQueueManager;
            
            size_t id;      // Id of this pop queue operation ( given by the PopQueueManager )
            
            network::PopQueue *pq;
            size_t delilahId;
            int fromId ;
            
            // Tasks that are currently running
            std::set<size_t> running_tasks;
            
            bool finished;
            
        public:

            au::ErrorManager error;
            
            PopQueue( const network::PopQueue& _pq , size_t _delilahId, int _fromId  );
            
            ~PopQueue()
            {
                delete pq;
            }
            
            void run( Queue * q );
            
            std::string getStatus();
            
            void sendFinalMessage();
            
            void sendMessage( engine::Buffer *buffer );

            void check();

            int getChannel()
            {
                return pq->target().channel();
            }
            
            std::string getQueue()
            {
                return pq->target().queue();
            }
            
            std::string getParserOut()
            {
                return pq->parserout();
            }
            
            
            void notifyFinishTask( size_t task_id , au::ErrorManager *_error )
            {
                running_tasks.erase( task_id );
                
                error.set( _error );
                
                check();
            }
            
        };
    }
}


#endif




#include "logMsg/logMsg.h"          // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager

#include "PopQueue.h"               // PopQueue

#include "PopQueueManager.h"        // Own interface

namespace samson {
    namespace stream {

        
        void PopQueueManager::add( PopQueue *pq  )
        {
            pq->id = id++;
            popQueues.insertInMap( pq->id , pq );
        }
        
        std::string PopQueueManager::getStatus()
        {
            
            std::ostringstream output;
            
            output << "Pop-Queues:\n";
            
            au::map< size_t , PopQueue >::iterator q;
            for ( q = popQueues.begin() ; q != popQueues.end() ; q++)
                output <<  au::Format::indent( q->second->getStatus()  ) << "\n";
            
            return output.str();
            
        }

        
        void PopQueueManager::notifyFinishTask( size_t pop_queue_id , size_t task_id , au::ErrorManager *error )
        {
            
            PopQueue*pq = popQueues.findInMap( pop_queue_id );
            
            if( pq )
                pq->notifyFinishTask( task_id , error );
            else
                LM_W(("Received a finish task message for pop queue %lu for an unknown taskl %lu", pop_queue_id,  task_id ));
            
        }
        
        
    }
}


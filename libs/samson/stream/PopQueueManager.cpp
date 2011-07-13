

#include "logMsg/logMsg.h"          // LM_W

#include "au/ErrorManager.h"        // au::ErrorManager

#include "engine/Engine.h"                  // engine::Engine
#include "engine/Notification.h"          // engine::Notification

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_check_finish_tasks

#include "PopQueue.h"               // PopQueue

#include "PopQueueManager.h"        // Own interface


namespace samson {
    namespace stream {

        
        
        PopQueueManager::PopQueueManager()
        {
            id = 1;    // Init the id counter
            
            listen(notification_samson_worker_check_finish_tasks);
        }

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


        void PopQueueManager::notify( engine::Notification* notification )
        {
            if( !notification->isName( notification_samson_worker_check_finish_tasks ) )
            {
                LM_W(("Unexpected notification received at PopQueueManager"));
                return;
            }
            
            // Clear finish pop queues tasks
            clearFinishPopQueues();
            
        }
        
        
        void PopQueueManager::clearFinishPopQueues()
        {
            std::vector<size_t> ids;
            
            au::map< size_t , PopQueue >::iterator q;
            for ( q = popQueues.begin() ; q != popQueues.end() ; )
            {
                if ( q->second->finished )
                {
                    // Remove the object contained here
                    delete q->second;
                    
                    // Remove the element in the map
                    popQueues.erase(q++);
                }
                else
                    ++q;
            }
            
        }
        
        
    }
}


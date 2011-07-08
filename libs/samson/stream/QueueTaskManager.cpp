

#include "QueueTaskManager.h"   // Own interface
#include "QueueTask.h"          // samson::stream::QueueTask
#include <sstream>              // std::ostringstream


#include "engine/Notification.h"        // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager

#include "samson/common/NotificationMessages.h"         // notification_process_request


#include "QueuesManager.h"   // QueueManager

namespace samson {
    namespace stream {
        
        QueueTaskManager::QueueTaskManager( QueuesManager* _qm )
        {
            id = 1;
            qm = _qm;
        }
        
        size_t QueueTaskManager::getNewId()
        {
            return id++;
        }
        
        // Insert a queue in the system
        
        void QueueTaskManager::add( QueueTask* task )
        {
            queueTasks.push_back( task );
            
            // Check if it is necessary to run a task
            runTasksIfNecessary();
        }
        
        std::string QueueTaskManager::getStatus()
        {
            std::ostringstream output;

            output << "Pending tasks:\n";
            au::list< QueueTask >::iterator i;
            for ( i = queueTasks.begin() ; i!= queueTasks.end() ; i++ )
                output <<  au::Format::indent( (*i)->getStatus() ) << "\n";

            {
                output << "Running tasks:\n";
                au::map< size_t, QueueTask >::iterator i;
                for ( i = runningTasks.begin() ; i!= runningTasks.end() ; i++ )
                {
                    QueueTask*tmp = i->second;
                    output << au::Format::indent( tmp->getStatus() ) << "\n";
                }
            }
            
            return output.str();
        }

        
        void QueueTaskManager::runTasksIfNecessary()
        {
            while( true )
            {
                
                if( queueTasks.size() == 0)
                    return; // No more pending task to be executed
                
                QueueTask * task = queueTasks.front();  // Take the front task
                
                if( task->lock() )
                {
                    QueueTask * _task = queueTasks.extractFront();
                    
                    if( task != _task )
                        LM_X(1, ("Internal error. Forbident concurrent access to Queue Tasks"));

                    // Insert in the running vector
                    runningTasks.insertInMap( _task->id , _task ); 

                    // Add this process item ( note that a notification will be used to notify when finished )
                    
                    engine::ProcessManager::shared()->add( _task->getStreamProcess() , getEngineId() );
                }
                
            }
            
        }
        
        void QueueTaskManager::notify( engine::Notification* notification )
        {
            if ( notification->isName(notification_process_request_response) )
            {
                
                //LM_M(("Notified finish task queue_name=%s // id=%lu" , queue_name.c_str() , _id));
                
                // Extract the object to not be automatically removed
                notification->extractObject();
                
                size_t _id       = notification->environment.getSizeT("id", 0);
                QueueTask *_task = runningTasks.extractFromMap(_id);
                
                if( _task )
                {
                    
                    // Notify that this stream task is finished
                    qm->notifyFinishTask( _task );

                    //LM_M(("StreamTask with id %lu finished", _id));
                    _task->unlock();
                    _task->release();
                    
                    //LM_M(("Destroying task"));
                    delete _task;
                    
                    
                }
                else
                    LM_W(("Notification of a finish item at QueueTaskManager, but task %lu not found in the running task list " , _id));
                
                
            }
        }
        

        
        
    }
    
}
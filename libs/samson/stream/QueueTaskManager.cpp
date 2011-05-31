

#include "QueueTaskManager.h"   // Own interface
#include "QueueTask.h"          // samson::stream::QueueTask
#include <sstream>              // std::ostringstream

#include "engine/ProcessManager.h"  // notification_process_request

namespace samson {
    namespace stream {
        
        QueueTaskManager::QueueTaskManager()
        {
            id = 1;
            // Ready to receive notifications
            listen( notification_process_request_response );    
        }
        
        
        // Insert a queue in the system
        
        void QueueTaskManager::add( QueueTask* task )
        {
            task->id = id++;    // Set the id of this task
            queueTasks.push_back( task );
            
            // Check if it is necessary to run a task
            runTasksIfNecessary();
        }
        
        std::string QueueTaskManager::getStatus()
        {
            std::ostringstream output;

            output << "\tPending tasks:\n";
            au::list< QueueTask >::iterator i;
            for ( i = queueTasks.begin() ; i!= queueTasks.end() ; i++ )
                output << "\t\t" << (*i)->getStatus() << "\n";

            {
                output << "\tRunning tasks:\n";
                au::map< size_t, QueueTask >::iterator i;
                for ( i = runningTasks.begin() ; i!= runningTasks.end() ; i++ )
                {
                    QueueTask*tmp = i->second;
                    output << "\t\t" << tmp->getStatus() << "\n";
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

                    // Notify to run this process item
                    engine::Notification *notification = new engine::Notification( notification_process_request , _task->getStreamProcess() );
                    notification->environment.set("target", "QueueTaskManager");
                    notification->environment.setSizeT("id", _task->id );
                    engine::Engine::add( notification );
                }
                
            }
            
        }
        
        void QueueTaskManager::notify( engine::Notification* notification )
        {
            if ( notification->isName(notification_process_request_response) )
            {
                size_t _id = notification->environment.getSizeT("id", 0);

                // Extract the object to not be automatically removed
                notification->extractObject();
                
                QueueTask *_task = runningTasks.extractFromMap(_id);
                
                if( _task )
                {
                    LM_M(("StreamTask with id %lu finished", _id));
                    _task->unlock();
                    _task->release();
                    
                    LM_M(("Destroying task"));
                    delete _task;
                    
                }
                else
                    LM_W(("Notification of a finish item at QueueTaskManager, but task %lu not found in the running task list " , _id));
            }
        }
        
        bool QueueTaskManager::acceptNotification( engine::Notification* notification )
        {
            if ( notification->environment.get("target", "") != "QueueTaskManager" )
                return false;
            
            return true;
            
        }

        
        
    }
    
}
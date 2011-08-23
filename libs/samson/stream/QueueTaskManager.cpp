

#include "QueueTaskManager.h"   // Own interface
#include "QueueTask.h"          // samson::stream::QueueTask
#include <sstream>              // std::ostringstream


#include "engine/Notification.h"        // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager

#include "samson/common/NotificationMessages.h"         // notification_process_request


#include "QueuesManager.h"   // QueueManager

#include "PopQueue.h"           // PopQueueTasks

#define notification_run_stream_tasks_if_necessary "notification_run_stream_tasks_if_necessary"

namespace samson {
    namespace stream {
        
        QueueTaskManager::QueueTaskManager( QueuesManager* _qm )
        {
            id = 1;
            qm = _qm;
          
            
            // Periodic notification to check if tasks are ready
            engine::Notification *notification = new engine::Notification(notification_run_stream_tasks_if_necessary);
            engine::Engine::shared()->notify( notification, 1 );
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
        
        void QueueTaskManager::add( PopQueueTask* task )
        {
            popQueueTasks.push_back( task );
            
            // Check if it is necessary to run a task
            runPopTasksIfNecessary();
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
                
                if( task->ready() )
                {
                    // Extract the task from the queue of pending tasks
                    QueueTask * _task = queueTasks.extractFront();

                    // Stupid check ;)
                    if( task != _task )
                        LM_X(1, ("Internal error. Forbident concurrent access to Queue Tasks"));

                    // Insert in the running vector
                    runningTasks.insertInMap( _task->id , _task ); 

                    // Add this process item ( note that a notification will be used to notify when finished )
                    engine::ProcessManager::shared()->add( _task , getEngineId() );
                }
                else
                    return; // The next task is not ready

            }
            
        }

        void QueueTaskManager::runPopTasksIfNecessary()
        {
            while( true )
            {
                
                if( popQueueTasks.size() == 0)
                    return; // No more pending task to be executed
                
                PopQueueTask * task = popQueueTasks.front();  // Take the front task
                
                if( task->ready() )
                {
                    // Extract the task from the queue of pending tasks
                    PopQueueTask * _task = popQueueTasks.extractFront();
                    
                    // Stupid check ;)
                    if( task != _task )
                        LM_X(1, ("Internal error. Forbident concurrent access to Queue Tasks"));
                    
                    // Insert in the running vector
                    runningPopQueueTasks.insertInMap( _task->id , _task ); 
                    
                    // Add this process item ( note that a notification will be used to notify when finished )
                    engine::ProcessManager::shared()->add( _task , getEngineId() );
                }
                else
                    return; // The next task is not ready
                
            }
            
        }
        
        void QueueTaskManager::notify( engine::Notification* notification )
        {
            if ( notification->isName(notification_process_request_response) )
            {
                
                //LM_M(("Notified finish task queue_name=%s // id=%lu" , queue_name.c_str() , _id));
                
                // Extract the object to not be automatically removed
                notification->extractObject();
                
                std::string type = notification->environment.get("type", "no-type");
                size_t _id       = notification->environment.getSizeT("id", 0);
                
                if( type == "pop_queue_task" )
                {
                    PopQueueTask *_task = runningPopQueueTasks.extractFromMap(_id);
                    
                    if( _task )
                    {
                        
                        // Notify that this stream task is finished
                        qm->notifyFinishTask( _task );
                        
                        //LM_M(("Destroying task"));
                        delete _task;
                        
                        
                    }
                    else
                        LM_W(("Notification of a finish item at QueueTaskManager, but task %lu not found in the running task list " , _id));
                    
                }
                else
                {
                
                    QueueTask *_task = runningTasks.extractFromMap(_id);
                    
                    if( _task )
                    {
                        
                        // Notify that this stream task is finished
                        qm->notifyFinishTask( _task );
                        
                        //LM_M(("Destroying task"));
                        delete _task;
                        
                        
                    }
                    else
                        LM_W(("Notification of a finish item at QueueTaskManager, but task %lu not found in the running task list " , _id));
                    
                }
                
            }
            
            
            if( notification->isName( notification_run_stream_tasks_if_necessary ) )
            {
                runTasksIfNecessary();
                runPopTasksIfNecessary();
            }
               
        }
        
        // Get information for monitorization
        void QueueTaskManager::getInfo( std::ostringstream& output)
        {
            {
                au::map< size_t , QueueTask >::iterator q;
                for (q = runningTasks.begin() ; q != runningTasks.end() ; q++)
                    q->second->getInfo( output );
            }
            
            {
                au::list< QueueTask >::iterator q;
                for (q = queueTasks.begin() ; q!= queueTasks.end() ; q++)
                    (*q)->getInfo(output);
            }
            
            
            
        }
        
        
        
    }
    
}
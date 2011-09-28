

#include "QueueTaskManager.h"   // Own interface
#include "QueueTask.h"          // samson::stream::QueueTask
#include <sstream>              // std::ostringstream


#include "engine/Notification.h"        // engine::Notification
#include "engine/ProcessManager.h"      // engine::ProcessManager

#include "samson/common/NotificationMessages.h"         // notification_process_request
#include "samson/common/SamsonSetup.h"                      // SamsonSetup

#include "StreamManager.h"          // StreamManager
#include "SystemQueueTask.h"        // SystemQueueTask
#include "PopQueue.h"               // PopQueueTasks
#include "PopQueueTask.h"           // PopQueueTasks

#define notification_run_stream_tasks_if_necessary "notification_run_stream_tasks_if_necessary"

namespace samson {
    namespace stream {
        
        QueueTaskManager::QueueTaskManager( StreamManager* _streamManager )
        {
            id = 1;
            streamManager = _streamManager;
            
            listen(notification_run_stream_tasks_if_necessary);
            
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
            reviewPendingQueueTasks();
        }
        
        void QueueTaskManager::add( SystemQueueTask* task )
        {
            systemQueueTasks.push_back( task );
            
            // Check if it is necessary to run a task
            reviewPendingQueueTasks();
        }

        void QueueTaskManager::notify( engine::Notification* notification )
        {
            if ( notification->isName(notification_process_request_response) )
            {
                
                // Extract the object to not be automatically removed ( this is a queueTask or a systemQueueTask )
                notification->extractObject();

                // Get the identifier of this
                size_t _id       = notification->environment.getSizeT("system.queue_task_id", 0);
                
                bool is_system_queue_task = (notification->environment.get("system.system_queue_task", "no") == "yes" );
                
                if( is_system_queue_task )
                {
                    // Recover the running SystemQueueTask
                    SystemQueueTask *_task = runningSystemQueueTasks.extractFromMap( _id );
                    
                    if( _task )
                    {
                        // Final process for this task
                        _task->finalize( streamManager );
                        
                        // Notify that this stream task is finished
                        streamManager->notifyFinishTask( _task );
                        
                        //LM_M(("Destroying task"));
                        delete _task;
                        
                    }
                    else
                        LM_W(("Notification of a finish pop_queue at QueueTaskManager, but task %lu not found in the running task list " , _id));
                    
                }
                else
                {
                
                    QueueTask *_task = runningTasks.extractFromMap(_id);
                    
                    if( _task )
                    {
                        
                        // Final process for this task
                        _task->finalize( streamManager );
                        
                        // Notify that this stream task is finished
                        streamManager->notifyFinishTask( _task );
                        
                        //LM_M(("Destroying task"));
                        delete _task;
                        
                        
                    }
                    else
                        LM_W(("Notification of a finish item at QueueTaskManager, but task %lu not found in the running task list " , _id));
                    
                }
                
            }
            
            
            if( notification->isName( notification_run_stream_tasks_if_necessary ) )
            {
                reviewPendingQueueTasks();
            }
            
        }
        
        void QueueTaskManager::reviewPendingQueueTasks()
        {
            int num_processors = SamsonSetup::shared()->getInt("general.num_processess"); 
            int num_running_tasks = (int)( runningTasks.size() + runningTasks.size() );
            
            while( num_running_tasks < (int)(1.5*(double)num_processors) )
            {
                
                bool runReturn = runNextQueueTasksIfNecessary();
                bool runReturn2 = runNextSystemQueueTasksIfNecessary();
                
                if( !runReturn && !runReturn2 )
                    return; // Nothing more to schedule
            }
            
        }
        
        bool QueueTaskManager::runNextQueueTasksIfNecessary()
        {
            
            if( queueTasks.size() == 0)
                return false; // No more pending task to be executed
            
            QueueTask * task = queueTasks.front();  // Take the front task
            
            if( task->ready() )
            {
                // Extract the task from the queue of pending tasks
                QueueTask * _task = queueTasks.extractFront();
                
                // Stupid check ;)
                if( task != _task )
                    LM_X(1, ("Internal error. Forbident concurrent access to Queue Tasks"));
                
                // Insert in the running vector
                size_t task_id = _task->getId();
                runningTasks.insertInMap( task_id , _task ); 
                
                // Add this process item ( note that a notification will be used to notify when finished )
                engine::ProcessManager::shared()->add( _task , getEngineId() );
                
                return true;
            }
            else
                return false; // The next task is not ready
            
            
        }
        
        bool QueueTaskManager::runNextSystemQueueTasksIfNecessary()
        {
            
            if( systemQueueTasks.size() == 0)
                return false; // No more pending task to be executed
            
            SystemQueueTask * task = systemQueueTasks.front();  // Take the front task
            
            if( task->ready() )
            {
                // Extract the task from the queue of pending tasks
                SystemQueueTask * _task = systemQueueTasks.extractFront();
                
                // Stupid check ;)
                if( task != _task )
                    LM_X(1, ("Internal error. Forbident concurrent access to Queue Tasks"));
                
                // Insert in the running vector
                size_t task_id = _task->getId();
                runningSystemQueueTasks.insertInMap( task_id , _task ); 
                
                // Add this process item ( note that a notification will be used to notify when finished )
                engine::ProcessManager::shared()->add( _task , getEngineId() );
                return true;
            }
            else
                return false; // The next task is not ready
            
        }
        
        
        // Get information for monitorization
        void QueueTaskManager::getInfo( std::ostringstream& output)
        {
            au::xml_iterate_list(output, "queue_tasks", queueTasks);
            au::xml_iterate_map(output, "running_queue_tasks", runningTasks);

            
            au::xml_iterate_list(output, "system_queue_tasks", systemQueueTasks);
            au::xml_iterate_map(output, "running_system_queue_tasks", runningSystemQueueTasks);
            
        }
        
        
        
    }
    
}

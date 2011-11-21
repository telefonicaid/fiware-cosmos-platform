

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
            task->setQueueTaskState("Queued");
            queueTasks.push_back( task );
            
            // Check if it is necessary to run a task
            reviewPendingQueueTasks();
        }
        
        void QueueTaskManager::add( SystemQueueTask* task )
        {
            task->setQueueTaskState("Queued");
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
                
                LM_T(LmtBlockManager, ("Notification of a finish at QueueTaskManager,  task %lu " , _id));

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
        
        bool QueueTaskManager::hasEnougthTasks()
        {
            int num_processors      = SamsonSetup::shared()->getInt("general.num_processess"); 
            int num_running_tasks   = (int)( runningTasks.size() + runningSystemQueueTasks.size() + systemQueueTasks.size() + queueTasks.size() );
            
            //LM_M(("hasEnougthTasks %d / %d" , num_running_tasks , num_processors ));
            
            return ( num_running_tasks >= num_processors );
        }
        
        void QueueTaskManager::reviewPendingQueueTasks()
        {
            int num_processors      = SamsonSetup::shared()->getInt("general.num_processess"); 
            int num_running_tasks   = (int)( runningTasks.size() + runningSystemQueueTasks.size() );
            
            int max_running_operations =  (int)( num_processors );
            
            while( num_running_tasks < max_running_operations )
            {
                //LM_M(("Scheduling since running rask %d < %d", (int) num_running_tasks , (int) max_running_operations));
                
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
            
            // Trying to search for the first task ready, instead of stopping at the front
            for ( au::list<QueueTask>::iterator t = queueTasks.begin() ; t != queueTasks.end() ; t++ )
            {


            //QueueTask * task = queueTasks.front();  // Take the front task
            	QueueTask * task = *t;  // Take the front task
            
            if( task->ready() )
            {
                // Extract the task from the queue of pending tasks
                //QueueTask * _task = queueTasks.extractFront();
            	QueueTask * _task = queueTasks.extractFromList(task);
                
                // Stupid check ;)
                if( task != _task )
                    LM_X(1, ("Internal error. Forbidden concurrent access to Queue Tasks"));
                
                // Insert in the running vector
                size_t task_id = _task->getId();
                runningTasks.insertInMap( task_id , _task ); 
                
                _task->setQueueTaskState("Scheduled");
                
                LM_T(LmtBlockManager, ("Scheduled task %lu " , task_id));

                // Add this process item ( note that a notification will be used to notify when finished )
                engine::ProcessManager::shared()->add( _task , getEngineId() );
                
                return true;
            }
            //else
            //    return false; // The next task is not ready
            }
            return false;
            
            
        }
        
        bool QueueTaskManager::runNextSystemQueueTasksIfNecessary()
        {
            
            if( systemQueueTasks.size() == 0)
                return false; // No more pending task to be executed
            
            // Trying to search for the first task ready, instead of stopping at the front
            for ( au::list<SystemQueueTask>::iterator t = systemQueueTasks.begin() ; t != systemQueueTasks.end() ; t++ )
            {



            //SystemQueueTask * task = systemQueueTasks.front();  // Take the front task
            	SystemQueueTask * task = *t;
            
            if( task->ready() )
            {
                // Extract the task from the queue of pending tasks
                //SystemQueueTask * _task = systemQueueTasks.extractFront();
            	SystemQueueTask * _task = systemQueueTasks.extractFromList(task);
                
                // Stupid check ;)
                if( task != _task )
                    LM_X(1, ("Internal error. Forbidden concurrent access to Queue Tasks"));
                
                // Insert in the running vector
                size_t task_id = _task->getId();
                runningSystemQueueTasks.insertInMap( task_id , _task ); 
                
                _task->setQueueTaskState("Scheduled");

                LM_T(LmtBlockManager, ("Scheduled task %lu " , task_id));

                // Add this process item ( note that a notification will be used to notify when finished )
                engine::ProcessManager::shared()->add( _task , getEngineId() );
                return true;
            }
            //else
            //    return false; // The next task is not ready
            }
            return false;
            
        }
        
        
        // Get information for monitoring
        void QueueTaskManager::getInfo( std::ostringstream& output)
        {
            au::xml_iterate_map(output, "running_queue_tasks", runningTasks);
            au::xml_iterate_map(output, "running_system_queue_tasks", runningSystemQueueTasks);
            
            au::xml_iterate_list(output, "queue_tasks", queueTasks);
            au::xml_iterate_list(output, "system_queue_tasks", systemQueueTasks);
            
        }
        
        bool QueueTaskManager::isActive()
        {
            
            if( queueTasks.size() > 0 )
                return true;
            if( runningTasks.size() > 0 )
                return true;
            if( systemQueueTasks.size() > 0 )
                return true;
            if( runningSystemQueueTasks.size() > 0 )
                return true;
            
            return false;
            
        }
        
        void QueueTaskManager::reset()
        {
            queueTasks.clearList();
            systemQueueTasks.clearList();   
        }

        
    }
}

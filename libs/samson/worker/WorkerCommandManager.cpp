

#include "engine/Notification.h"
#include "engine/Engine.h"

#include "samson/common/NotificationMessages.h"

#include "WorkerCommand.h"
#include "WorkerCommandManager.h" // Own interface


namespace samson {
    
    WorkerCommandManager::WorkerCommandManager( SamsonWorker * _samsonWorker )
    {
        // Keep pointer to the worker
        samsonWorker = _samsonWorker;
        
        // Init identifier for the command
        worker_task_id = 1;
        
        // Schedule a periodic notification ( every 10 seconds )
        {
            engine::Notification *notification = new engine::Notification(notification_review_worker_command_manager);
            engine::Engine::shared()->notify( notification, 5 );
        }
        
    }
    
    void WorkerCommandManager::addWorkerCommand( WorkerCommand *workerCommand )
    {
        // Set the internal pointer to stream manager
        workerCommand->setSamsonWorker( samsonWorker );
        
        size_t id = worker_task_id++;
        workerCommands.insertInMap( id , workerCommand );
        
        // First run of this worker command
        workerCommand->run();
    }
    
    void WorkerCommandManager::notify( engine::Notification* notification )
    {
        
        if ( notification->isName(notification_review_worker_command_manager) )
        {
            // Remove finished worker tasks elements
            workerCommands.removeInMapIfFinished();
            
            // Review all WorkerCommand is necessary
            au::map< size_t , WorkerCommand >::iterator it_workerCommands; 
            for( it_workerCommands = workerCommands.begin() ; it_workerCommands != workerCommands.end() ; it_workerCommands++ )
                it_workerCommands->second->run();   // Excute if necessary
            
            
            return;
        }
        
        
    }

    // Get information for monitoring
    void WorkerCommandManager::getInfo( std::ostringstream& output)
    {
        output << "<worker_command_manager>\n";
        
        // WorkerCommands
        au::xml_iterate_map(output, "worker_commands", workerCommands );
        
        output << "</worker_command_manager>\n";
        
    }
}
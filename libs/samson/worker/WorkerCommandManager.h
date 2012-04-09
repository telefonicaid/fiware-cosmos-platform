
#ifndef _H_STREAM_WORKER_COMMAND_MANAGER
#define _H_STREAM_WORKER_COMMAND_MANAGER

/* ****************************************************************************
 *
 * FILE                      WorkerCommandManager.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * 
 *
 */

#include "au/containers/map.h"

#include "samson/common/samson.pb.h"
#include "samson/common/Visualitzation.h"

#include "engine/Object.h"

namespace samson {
    
    class SamsonWorker;
    class Info;
    
    class WorkerCommand;
    
    // Manager of WorkerCommand elements
    class WorkerCommandManager : public engine::Object
    {
        
        // Pointer to the global samson worker
        SamsonWorker * samsonWorker;
        
        // Manager of the current "stream-tasks" running on this worker
        au::map< size_t , WorkerCommand > workerCommands; 
        
        // Internal counter to WorkerTasks
        size_t worker_task_id;

    public:
        
        // Constructor
        WorkerCommandManager( SamsonWorker * _samsonWorker);
        
        // Add a worker command to this stream manager
        void addWorkerCommand( WorkerCommand *workerCommand );

        // Cancel worker command 
        bool cancel( std::string worker_command_id );
        
        // Notification system from engine
        void notify( engine::Notification* notification );

        // XML report 
        void getInfo( std::ostringstream& output);

        // Get Collection of worker_commands ( for ls_worker_commands )
        samson::network::Collection* getCollectionOfWorkerCommands( Visualization * visualization );
        
        
    };
    
}


#endif
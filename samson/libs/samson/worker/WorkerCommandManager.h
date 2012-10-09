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

#include <map>
#include <string>

#include "au/containers/map.h"
#include "au/containers/SharedPointer.h"

#include "engine/NotificationListener.h"

#include "samson/common/samson.pb.h"
#include "samson/common/Visualitzation.h"

namespace samson {
class SamsonWorker;
class Info;

class WorkerCommand;

// Manager of WorkerCommand elements
class WorkerCommandManager : public engine::NotificationListener {
    // Pointer to the global samson worker
    SamsonWorker *samsonWorker;

    // Manager of the worker_commands
    au::map<size_t, WorkerCommand> workerCommands;

    // Internal counter to WorkerTasks
    size_t worker_task_id;

  public:
    // Constructor
    explicit WorkerCommandManager(SamsonWorker *_samsonWorker);

    // Add a worker command to this stream manager
    void Add(WorkerCommand *workerCommand);

    // Cancel worker command
    bool Cancel(std::string worker_command_id);

    // Notification system from engine
    void notify(engine::Notification *notification);

    // Get Collection of worker_commands ( for ls_worker_commands )
    au::SharedPointer<gpb::Collection> GetCollectionOfWorkerCommands(const Visualization& visualization);
};
}

#endif  // ifndef _H_STREAM_WORKER_COMMAND_MANAGER

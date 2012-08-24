
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

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

#include "engine/NotificationListener.h"

namespace samson {
class SamsonWorker;
class Info;

class WorkerCommand;

// Manager of WorkerCommand elements
class WorkerCommandManager : public engine::NotificationListener {
  // Pointer to the global samson worker
  SamsonWorker *samsonWorker;

  // Manager of the worker_commands
  au::map< size_t, WorkerCommand > workerCommands;

  // Internal counter to WorkerTasks
  size_t worker_task_id;

public:

  // Constructor
  WorkerCommandManager(SamsonWorker *_samsonWorker);

  // Add a worker command to this stream manager
  void addWorkerCommand(WorkerCommand *workerCommand);

  // Cancel worker command
  bool cancel(std::string worker_command_id);

  // Notification system from engine
  void notify(engine::Notification *notification);

  // XML report
  void getInfo(std::ostringstream& output);

  // Get Collection of worker_commands ( for ls_worker_commands )
  samson::gpb::Collection *getCollectionOfWorkerCommands(const Visualization& visualization);
};
}


#endif  // ifndef _H_STREAM_WORKER_COMMAND_MANAGER
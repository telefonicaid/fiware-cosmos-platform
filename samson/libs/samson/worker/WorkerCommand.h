#ifndef _H_STREAM_WORKER_COMMAND
#define _H_STREAM_WORKER_COMMAND

/* ****************************************************************************
 *
 * FILE                      WorkerCommand.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 *
 *
 */

#include <sstream>
#include <string>

#include "au/CommandLine.h"                 // au::CommandLine
#include "au/containers/map.h"              // au::map
#include "au/containers/vector.h"
#include "au/string.h"                      // au::Format

#include "engine/NotificationListener.h"    // engine::NotificationListener

#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"        // network::...
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/module/ModulesManager.h"   // samson::ModulesManager

#include "samson/stream/WorkerTaskManager.h"  // samson::stream::WorkerTaskManager

#include "engine/Buffer.h"                  // engine::Buffer
#include "engine/NotificationListener.h"    // engine::NotificationListener


namespace samson {
class SamsonWorker;
class Info;


class Queue;
class Block;
class BlockList;
namespace stream {
class StreamOperationBase;
}

// Worker Tasks is an action working on this worker
// It is basically a set of individual queue-tasks

class WorkerCommand : public engine::NotificationListener {
  std::string worker_command_id;       // Unique identifier ( used to associate all items associated with this worker_command )

  bool notify_finish;                                               // Flag to mark if it is necessary to notify when finish
  gpb::WorkerCommand *originalWorkerCommand;                        // Copy of the original message

  // Identifiers to notify when finished
  size_t delilah_id;                                                // Delilah identifier of this task
  size_t delilah_component_id;                                      // Identifier inside delilah

  // Pointer to the samsonWorker
  SamsonWorker *samsonWorker;

  // Error management
  au::ErrorManager error;

  // Command to run
  std::string command;

  // Environment properties
  Environment enviroment;

  // Flag to indicate that this command is still pending to be executed
  bool pending_to_be_executed;

  // Flag to indicate that this worker-command has been completed and a message has been sent back to who sent the request
  bool finished;

  // Number of pending processes
  int num_pending_processes;
  int num_pending_disk_operations;

  // Collections added in the response message
  au::vector< samson::gpb::Collection > collections;


  friend class WorkerCommandManager;

public:

  WorkerCommand(std::string worker_command_id
                , size_t _delilah_id
                , size_t _delilah_component_id
                , const gpb::WorkerCommand& _command);

  ~WorkerCommand();

  void setSamsonWorker(SamsonWorker *_samsonWorker);
  bool isFinished();

  void runCommand(std::string command, au::ErrorManager *error);
  void run();

  void notify(engine::Notification *notification);

  // XML report
  void getInfo(std::ostringstream& output);

  // Fill a collection record
  void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

private:

  void finishWorkerTaskWithError(std::string error_message);
  void finishWorkerTask();

  // Auxiliar operation
  stream::StreamOperationBase *getStreamOperation(Operation *op);

  // Create collection for buffers (Memory manager )
  gpb::Collection *getCollectionOfBuffers(const Visualization& visualization);

  // Function to check everything is finished
  void checkFinish();
};
}

#endif  // ifndef _H_STREAM_WORKER_COMMAND
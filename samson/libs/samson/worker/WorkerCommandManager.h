/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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

#include "au/containers/SharedPointer.h"
#include "au/containers/map.h"
#include "engine/NotificationListener.h"
#include "samson/common/Logs.h"
#include "samson/common/Visualitzation.h"
#include "samson/common/samson.pb.h"

namespace samson {
class SamsonWorker;
class Info;

class WorkerCommand;

// Manager of WorkerCommand elements
class WorkerCommandManager : public engine::NotificationListener {
public:

  explicit WorkerCommandManager(SamsonWorker *samson_worker);
  ~WorkerCommandManager() {
    LOG_D(logs.cleanup, ("Calling ~WorkerCommandManager"));
  }

  // Add a worker command to this stream manager
  void Add(WorkerCommand *workerCommand);

  // Cancel worker command
  bool Cancel(std::string worker_command_id);

  // Notification system from engine
  void notify(engine::Notification *notification);

  // Get Collection of worker_commands ( for ls_worker_commands )
  au::SharedPointer<gpb::Collection> GetCollection(const Visualization& visualization);

private:

  // Pointer to the global samson worker
  SamsonWorker *samson_worker_;

  // Manager of the worker_commands
  au::map<size_t, WorkerCommand> worker_commands_;

  // Internal counter to WorkerTasks
  size_t worker_task_id_;
};
}

#endif  // ifndef _H_STREAM_WORKER_COMMAND_MANAGER

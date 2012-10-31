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
#include <vector>

#include "au/CommandLine.h"                 // au::CommandLine
#include "au/containers/map.h"              // au::map
#include "au/containers/SharedPointer.h"
#include "au/string/StringUtilities.h"                      // au::Format
#include "engine/NotificationListener.h"    // engine::NotificationListener
#include "samson/common/samson.pb.h"        // network::...
#include "samson/common/Visualitzation.h"
#include "samson/module/Environment.h"      // samson::Environment
#include "samson/module/ModulesManager.h"   // samson::ModulesManager
#include "samson/stream/WorkerTaskManager.h"  // samson::stream::WorkerTaskManager

namespace samson {
class SamsonWorker;
class Info;
class Queue;
class Block;
class BlockList;

// Worker Tasks is an action working on this worker
// It is basically a set of individual queue-tasks

class WorkerCommand : public engine::NotificationListener {
  public:
    WorkerCommand(SamsonWorker *_samsonWorker, std::string worker_command_id, size_t _delilah_id,
                  size_t _delilah_component_id, const gpb::WorkerCommand& _command);

    ~WorkerCommand();

    // engine notification system
    void notify(engine::Notification *notification);

    // Run command
    void RunCommand(std::string command, au::ErrorManager *error);
    void Run();

    // Accessor
    bool finished();

    // Fill a collection record
    void fill(samson::gpb::CollectionRecord *record, const Visualization& visualization);

    // Create collection for buffers
    au::SharedPointer<gpb::Collection> GetCollectionOfBuffers(const Visualization& visualization);

  private:
    void FinishWorkerTaskWithError(std::string error_message);   // Mark this command as finished with an error as a message
    void FinishWorkerTask();   // Mark this command as finished
    void CheckFinish();   // Function to check everything is finished

    std::string worker_command_id_;   // Unique identifier ( used to associate all items associated with this worker_command )

    bool notify_finish_;   // Flag to mark if it is necessary to notify when finish
    au::SharedPointer<gpb::WorkerCommand> originalWorkerCommand_;   // Copy of the original message

    // Identifiers to notify when finished
    size_t delilah_id_;   // Delilah identifier of this task
    size_t delilah_component_id_;   // Identifier inside delilah

    // Pointer to the samsonWorker
    SamsonWorker *samson_worker_;

    // Error management
    au::ErrorManager error_;

    // Command to run
    std::string command_;

    // Environment properties
    Environment enviroment_;

    // Flag to indicate that this command is still pending to be executed
    bool pending_to_be_executed_;

    // Flag to indicate that this worker-command has been completed and a message has been sent back to who sent the request
    bool finished_;

    // Number of pending processes
    int num_pending_processes_;
    int num_pending_disk_operations_;

    // Collections added in the response message
    std::vector<au::SharedPointer<samson::gpb::Collection> > collections_;

    friend class WorkerCommandManager;
};
}

#endif  // ifndef _H_STREAM_WORKER_COMMAND

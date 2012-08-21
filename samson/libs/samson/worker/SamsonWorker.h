#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
 *
 * FILE                     SamsonWorker.h
 *
 * DESCRIPTION			   Main class for the worker element
 *
 */

#include <iomanip>                              // setiosflags()
#include <iostream>                             // std::cout

#include "logMsg/logMsg.h"                      //

#include "au/console/Console.h"
#include "au/containers/SharedPointer.h"
#include "au/network/RESTService.h"
#include "au/network/RESTServiceCommand.h"

#include "engine/EngineElement.h"               // samson::EngineElement

#include "samson/stream/BlockManager.h"
#include "samson/stream/WorkerTaskManager.h"
#include "samson/stream/WorkerTaskManager.h"     // samson::stream::WorkerTaskManager

#include "samson/module/Operation.h"

#include "samson/common/KVHeader.h"
#include "samson/common/Macros.h"                       // exit(.)
#include "samson/common/NotificationMessages.h"
#include "samson/common/samson.pb.h"                    // samson::network::
#include "samson/common/samsonDirectories.h"    // SAMSON_WORKER_DEFAULT_PORT

#include "samson/zoo/DataModel.h"
#include "samson/zoo/SamsonWorkerController.h"

#include "samson/delilah/Delilah.h"
#include "samson/network/WorkerNetwork.h"
#include "samson/worker/DistributionBlocks.h"
#include "samson/worker/SamsonWorkerSamples.h"
#include "samson/worker/WorkerCommandManager.h"


namespace samson {
class NetworkInterface;
class Info;
class DistributionBlock;

namespace stream {
class WorkerTaskManager;
}

namespace worker {
class PushOperation;
class PushManager;
}

class SamsonWorker :
  public NetworkInterfaceReceiver,
  public engine::Object,
  public au::Console,
  public au::network::RESTServiceInterface {
  // Initial time stamp for this worker
  au::Cronometer cronometer;

  // Main connection with the zk
  zoo::Connection *zoo_connection_;

  // Cluster setup controller
  SamsonWorkerController *worker_controller;

  // Data model
  DataModel *data_model;

  // Auto-client for REST interface
  Delilah *delilah;

  // REST Service
  au::network::RESTService *rest_service;

  // Sampler for REST interface
  SamsonWorkerSamples samson_worker_samples;

  // Manager of push operations
  samson::worker::PushManager *push_manager;

  // Map of blocks recently created
  DistributionBlockManager *distribution_blocks_manager;

  // Manager for tasks
  stream::WorkerTaskManager *task_manager;

  // Network interface to communicate with the rest of the system
  WorkerNetwork *network_;

  // Manager of the "Worker commands"
  WorkerCommandManager *workerCommandManager;

  bool status_ready_to_process_;        // Flag to indicate if we are ready to process data
  std::string status_message_;          // Message why we are not ready

  // Internal components of samsonWorker
  friend class worker::PushOperation;
  friend class worker::PushManager;
  friend class WorkerCommand;
  friend class DistributionBlock;
  friend class stream::WorkerTaskManager;
  friend class SamsonWorkerSamples;
  friend class stream::StreamOperationInfo;
  friend class stream::WorkerTask;
  friend class DistributionBlockManager;
  friend class BlockRequest;
  // friend class stream::BlockRequestTask;

public:

  SamsonWorker(std::string zoo_host, int port, int web_port);
  ~SamsonWorker();

  // Reset worker ( when cluster setup chage )
  void ResetWorker();

  // Main function to review samson worker and all its elements
  // This function is preiodically called from engine
  void review();

  // Interface to receive Packets ( NetworkInterfaceReceiver )
  void receive(const PacketPointer& packet);

  // Notification from the engine about finished tasks
  void notify(engine::Notification *notification);

  // Get information for monitoring
  void getInfo(std::ostringstream& output);

  // au::Console ( debug mode with fg )
  void autoComplete(au::ConsoleAutoComplete *info);
  void evalCommand(std::string command);
  std::string getPrompt();

  // Send a trace to all delilahs
  void sendTrace(std::string type, std::string context, std::string message);

  // Get a collection with a single record with information for this worker...
  gpb::Collection *getWorkerCollection(const Visualization& visualization);

  // RESTServiceInterface
  void process(au::SharedPointer< au::network::RESTServiceCommand> command);
  void process_intern(au::SharedPointer< au::network::RESTServiceCommand> command);
  void process_delilah_command(std::string delilah_command, au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_node(au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_ilogging(au::SharedPointer<au::network::RESTServiceCommand> command);
  void process_logging(au::SharedPointer<au::network::RESTServiceCommand> command);

  void stop() {
    // Stop the rest waiting for all connections to finish ( even the thread for the listener )
    rest_service->StopService();
  }

  WorkerNetwork *network() {
    return network_;
  }

private:

  // Create zookeeper connection and related elements
  void Connect();

  // Check status of this worker
  void CheckStatus();

  // Generate a new block id
  size_t get_new_block_id();

  // Connection values ( to reconnect if connections fails down )
  std::string zoo_host_;
  int port_;
  int web_port_;
};
}

#endif // ifndef SAMSON_WORKER_H

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
#include "samson/worker/PushManager.h"
#include "samson/worker/SamsonWorkerRest.h"
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

class SamsonWorkerRest;

class SamsonWorker :
  public NetworkInterfaceReceiver,
  public engine::NotificationListener,
  public au::Console {
public:

  SamsonWorker(std::string zoo_host, int port, int web_port);
  ~SamsonWorker() {
  };                   // All internal components are shared pointers

  // Interface to receive Packets ( NetworkInterfaceReceiver )
  void receive(const PacketPointer& packet);

  // Notification from the engine about finished tasks
  void notify(engine::Notification *notification);

  // au::Console ( debug mode with fg )
  void autoComplete(au::ConsoleAutoComplete *info);
  void evalCommand(std::string command);
  std::string getPrompt();

  // Get a collection with a single record with information for this worker...
  gpb::Collection *getWorkerCollection(const Visualization& visualization);

  // Accessors to individual components of this worker
  au::SharedPointer<zoo::Connection> zoo_connection();
  au::SharedPointer<SamsonWorkerController> worker_controller();
  au::SharedPointer<DataModel> data_model();
  au::SharedPointer<WorkerNetwork> network();
  au::SharedPointer<SamsonWorkerRest> samson_worker_rest();
  au::SharedPointer<samson::worker::PushManager> push_manager();
  au::SharedPointer<DistributionBlockManager> distribution_blocks_manager();
  au::SharedPointer<stream::WorkerTaskManager> task_manager();
  au::SharedPointer<WorkerCommandManager> workerCommandManager();

private:


  // Reset worker ( when cluster setup chage )
  void ResetWorker();

  // Main function to review samson worker and all its elements
  // This function is preiodically called from engine
  void Review();

  // Create zookeeper connection and related elements
  void Connect();

  // Connection values ( to reconnect if connections fails down )
  std::string zoo_host_;
  int port_;
  int web_port_;

  // Initial time stamp for this worker
  au::Cronometer cronometer_;

  // Main elements of the worker
  au::SharedPointer<zoo::Connection> zoo_connection_;            // Main connection with the zk
  au::SharedPointer<SamsonWorkerController> worker_controller_;  // Cluster setup controller
  au::SharedPointer<DataModel> data_model_;                      // Data model
  au::SharedPointer<WorkerNetwork> network_;                     // Network manager to manage connections
  au::SharedPointer<SamsonWorkerRest> samson_worker_rest_;       // REST Service
  au::SharedPointer<samson::worker::PushManager> push_manager_;  // Manager of push operations
  au::SharedPointer<DistributionBlockManager> distribution_blocks_manager_;     // Map of blocks recently created
  au::SharedPointer<stream::WorkerTaskManager> task_manager_;    // Manager for tasks
  au::SharedPointer<WorkerCommandManager> workerCommandManager_;  // Manager of the "Worker commands"

  bool ready_;                                                   // Flag to indicate if this worker is ready
  std::string status_message_;       // Message why we are not ready
};
}

#endif  // ifndef SAMSON_WORKER_H

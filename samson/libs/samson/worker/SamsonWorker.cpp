#include <dirent.h>
#include <dlfcn.h>
#include <iconv.h>
#include <iostream>                               // std::cout ...
#include <sys/types.h>

#include "json.h"

#include "logMsg/logMsg.h"                        // lmInit, LM_*
#include "logMsg/traceLevels.h"                   // Trace Levels

#include "au/CommandLine.h"                       // CommandLine
#include "au/S.h"
#include "au/ThreadManager.h"
#include "au/string.h"                            // au::Format
#include "au/tables/pugixml.hpp"                  // pugixml
#include "au/time.h"                              // au::todayString

#include "au/network/RESTServiceCommand.h"

#include "au/log/LogToServer.h"

#include "engine/Notification.h"                  // engine::Notification

#include "samson/common/KVRange.h"
#include "samson/common/Macros.h"                 // EXIT, ...
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup

#include "samson/module/samsonVersion.h"          // SAMSON_VERSION

#include "samson/network/Message.h"               // Message
#include "samson/network/NetworkInterface.h"      // NetworkInterface
#include "samson/network/Packet.h"                // samson::Packet
#include "samson/network/WorkerNetwork.h"

#include "engine/MemoryManager.h"                 // samson::SharedMemory

#include "engine/DiskManager.h"                   // Notifications
#include "engine/DiskOperation.h"                 // samson::DiskOperation
#include "engine/Engine.h"                        // engine::Engine
#include "engine/ProcessManager.h"                // engine::ProcessManager
#include "samson/isolated/SharedMemoryManager.h"  // engine::SharedMemoryManager

// MemoryInput , MemoryOutputNetwork ,...

#include "samson/stream/Block.h"                  // samson::stream::Block
#include "samson/stream/BlockList.h"              // samson::stream::BlockList
#include "samson/stream/BlockManager.h"           // samson::stream::BlockManager

#include "samson/module/ModulesManager.h"         // samson::ModulesManager

#include "samson/network/NetworkInterface.h"      // samson::NetworkInterface

#include "samson/delilah/WorkerCommandDelilahComponent.h"

#include "samson/stream/WorkerTaskManager.h"
#include "samson/worker/DistributionBlocks.h"
#include "samson/worker/PushManager.h"
#include "samson/worker/SamsonWorker.h"  // Own interfce
#include "samson/worker/WorkerCommand.h"  // samson::stream::WorkerCommand

#define notification_samson_worker_take_sample "notification_samson_worker_take_sample"
#define notification_samson_worker_review      "notification_samson_worker_review"
#define notification_samson_worker_review2     "notification_samson_worker_review2"

extern int port;                         // Port where samsonWorker is started
extern int web_port;

namespace samson {
/* ****************************************************************************
 *
 * Constructor
 */

SamsonWorker::SamsonWorker(std::string zoo_host, int port, int web_port) : samson_worker_samples(this) {
  // Connection values
  zoo_host_ = zoo_host;
  port_ = port;
  web_port_ = web_port;

  // Default values
  zoo_connection_ = NULL;
  data_model = NULL;
  network_ = NULL;

  // Connect
  Connect();

  // Auto-client init in first REST connection
  delilah = NULL;   // Still testing cluster setup.... ( disabling temporary )
                    // delilah = new Delilah( "rest" );
                    // delilah->connect( au::str("localhost:%d" , port ) ); // Connect to myself

  // Push manager
  push_manager = new samson::worker::PushManager(this);

  // Init worker command manager
  workerCommandManager = new WorkerCommandManager(this);
  LM_T(LmtCleanup, ("Created workerCommandManager: %p", workerCommandManager));

  // Init distribution block
  distribution_blocks_manager = new DistributionBlockManager(this);

  // Random initialization
  srand(time(NULL));

  // Listen this notification to send traces
  listen(notification_samson_worker_send_trace);

  // Notification to update state
  listen(notification_update_status);
  {
    int update_period = samson::SamsonSetup::shared()->getInt("general.update_status_period");
    engine::Notification *notification = new engine::Notification(notification_update_status);
    engine::Engine::shared()->notify(notification, update_period);
  }

  {
    int check_finish_tasks_period = samson::SamsonSetup::shared()->getInt("worker.period_check_finish_tasks");
    engine::Notification *notification = new engine::Notification(notification_samson_worker_check_finish_tasks);
    engine::Engine::shared()->notify(notification, check_finish_tasks_period);
  }

  // Take samples for the REST interface
  listen(notification_samson_worker_take_sample);
  {
    engine::Notification *notification = new engine::Notification(notification_samson_worker_take_sample);
    engine::Engine::shared()->notify(notification, 1);
  }

  // Notification with rest commands
  listen("rest_operation_stream_manager");


  // General review of worker every second
  listen(notification_samson_worker_review);
  {
    engine::Notification *notification = new engine::Notification(notification_samson_worker_review);
    engine::Engine::shared()->notify(notification, 1);
  }

  listen(notification_samson_worker_review2);
  {
    engine::Notification *notification = new engine::Notification(notification_samson_worker_review2);
    engine::Engine::shared()->notify(notification, 5);
  }

  // Cluster setup changes
  listen("notification_cluster_info_changed");

  // Run REST interface
  rest_service = new au::network::RESTService(web_port, this);
  rest_service->InitService();

  // Review notification
  listen("samson_worker_review");
  engine::Engine::shared()->notify_extra(new engine::Notification("samson_worker_review"));

  // Manager of tasks
  task_manager =  new stream::WorkerTaskManager(this);

  // Init status of this worker
  ResetWorker();
}

void SamsonWorker::Connect() {
  while (true) {
    // Remove previous connection
    if (zoo_connection_) {
      delete zoo_connection_;
      zoo_connection_ = NULL;
    }

    if (network_) {
      delete network_;
      network_ = NULL;
    }

    if (data_model) {
      delete data_model;
      data_model = NULL;
    }

    // Create connection
    LM_T(LmtClusterSetup, ("Trying to connect with zk at %s", zoo_host_.c_str()));
    zoo_connection_ = new zoo::Connection(zoo_host_, "samson", "samson");

    int rc = zoo_connection_->WaitUntilConnected(5000);
    if (rc) {
      LM_W(("Not possible to connect with zk at %s (%s)"
            , zoo_host_.c_str()
            , zoo::str_error(rc).c_str()));
      continue;
    }

    // Make sure basic folders are created
    LM_T(LmtClusterSetup, ("Creating basic folders in zk. Jsut in case I am the first one..."));
    zoo_connection_->Create("/samson");
    zoo_connection_->Create("/samson/workers");

    // Main worker controller ( based on zookeeper )
    worker_controller = new SamsonWorkerController(zoo_connection_, port_, web_port_);
    rc = worker_controller->init();
    if (rc) {
      LM_W(("Error creating worker controller %s", zoo::str_error(rc).c_str()));
      continue;
    }

    // Data model
    data_model = new DataModel(zoo_connection_);


    // Recover cluster information for the network segment
    samson::gpb::ClusterInfo *cluster_info;
    size_t cluster_info_version;
    worker_controller->GetClusterInfo(&cluster_info, &cluster_info_version);

    // Dynamic network interface
    network_ = new WorkerNetwork(worker_controller->worker_id()
                                 , port
                                 , this
                                 , cluster_info
                                 , cluster_info_version);
    break;
  }
}

void SamsonWorker::ResetWorker() {
  // Remove whatever was schduled in process manager
  task_manager->reset();

  // Reset the status of this worker
  status_ready_to_process_ = false;
  status_message_ = "Initializing worker";
  CheckStatus();
}

SamsonWorker::~SamsonWorker() {
  LM_T(LmtCleanup, ("Deleting workerCommandManager: %p", workerCommandManager));
  delete workerCommandManager;

  if (worker_controller)
    delete worker_controller; if (data_model)
    delete data_model; if (network_)
    delete network_; if (rest_service)
    delete rest_service; if (task_manager)
    delete task_manager;
}

void SamsonWorker::review() {
  // Review distribution of blocks
  distribution_blocks_manager->Review();

  // Review task_manager if we are ready to process data
  if (status_ready_to_process_)
    task_manager->review_stream_operations();
}

/* ****************************************************************************
 *
 * SamsonWorker::receive -
 */

void SamsonWorker::receive(const PacketPointer& packet) {
  LM_T(LmtNetworkNodeMessages, ("SamsonWorker received %s ", packet->str().c_str()));


  // Type of message received
  Message::MessageCode msgCode = packet->msgCode;

  // --------------------------------------------------------------------
  // StatusReport
  // --------------------------------------------------------------------

  if (msgCode == Message::StatusReport)
    // LM_M(("Recieved status report message from %s" , packet->from.str().c_str() ));
    return;

  // --------------------------------------------------------------------
  // BlockRequest
  // --------------------------------------------------------------------

  if (msgCode == Message::BlockRequest) {
    if (!packet->message->has_block_id()) {
      LM_W(("Received a Message::BlockRequest without block id"));
      return;
    }

    if (packet->from.node_type == DelilahNode) {
      LM_W(("Received a Message::BlockRequest from a delilah node"));
      return;
    }

    size_t block_id  = packet->message->block_id();
    size_t worker_id = packet->from.id;

    // Schedule operations to send this block to this user
    if (stream::BlockManager::shared()->getBlock(block_id) == NULL) {
      LM_W(("Received a Message::BlockRequest for unknown block %lu", block_id));
      return;
    }

    // Add the task for this request
    task_manager->add_block_request_task(block_id, worker_id);

    return;
  }

  // --------------------------------------------------------------------
  // BlockRequest
  // --------------------------------------------------------------------

  if (msgCode == Message::PopBlockRequest) {
    if (!packet->message->has_block_id()) {
      LM_W(("Received an incorrect Message::PopBlockRequest. No block id"));
      return;
    }

    if (!packet->message->has_ranges()) {
      LM_W(("Received an incorrect Message::PopBlockRequest. No ranges"));
      return;
    }

    if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a Message::PopBlockRequest from a worker node"));
      return;
    }

    size_t block_id  = packet->message->block_id();
    const gpb::KVRanges ranges = packet->message->ranges();
    size_t delilah_id = packet->from.id;
    size_t delilah_component_id = packet->message->delilah_component_id();
    size_t pop_id = packet->message->pop_id();

    PacketPointer p(new Packet(Message::PopBlockRequestConfirmation, packet->from));
    p->message->set_delilah_component_id(packet->message->delilah_component_id());
    p->message->set_pop_id(packet->message->pop_id());

    // Schedule operations to send this block to this user
    if (stream::BlockManager::shared()->getBlock(block_id) == NULL) {
      p->message->mutable_error()->set_message("Unknown block");
    } else {
      // Schedule task
      au::SharedPointer<stream::WorkerSystemTask> task;
      task.Reset(new stream::PopBlockRequestTask(this
                                                 , task_manager->getNewId()
                                                 , block_id
                                                 , ranges
                                                 , delilah_id
                                                 , delilah_component_id
                                                 , pop_id));
      task_manager->Add(task.static_pointer_cast<stream::WorkerTaskBase>());
    }

    // Send confirmation packet
    network_->Send(p);

    return;
  }

  // --------------------------------------------------------------------
  // Duplicate blocks
  // --------------------------------------------------------------------

  if (msgCode == Message::DuplicateBlock) {
    if (!packet->message->has_block_id()) {
      LM_W(("Received a Message::DuplicateBlock without block id"));
      return;
    }

    if (packet->buffer() == NULL) {
      LM_W(("Received a Message::DuplicateBlock without a buffer"));
      return;
    }

    size_t block_id = packet->message->block_id();
    distribution_blocks_manager->CreateBlock(packet->buffer(), block_id);

    // Send message back
    PacketPointer p(new Packet(Message::DuplicateBlockResponse));
    p->message->set_block_id(block_id);
    p->to = packet->from;

    network_->Send(p);
    return;
  }

  if (msgCode == Message::DuplicateBlockResponse) {
    if (!packet->message->has_block_id()) {
      LM_W(("Received a Message::DuplicateBlockResponse with block id"));
      return;
    }
    size_t block_id = packet->message->block_id();
    size_t worker_id = packet->from.id;

    distribution_blocks_manager->ConfirmBlockDistribution(block_id, worker_id);
    return;
  }

  // --------------------------------------------------------------------
  // push messages
  // --------------------------------------------------------------------

  if (msgCode == Message::PushBlockCommit) {
    if (!packet->message->has_push_id()) {
      LM_W(("Received a push block message without the push_id"));
      return;   // No sense to answer with this error
    } else if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;   // No sense to answer with this error
    }

    // Use push manager to deal with this type of message
    size_t push_id = packet->message->push_id();
    push_manager->receive_push_block_commit(packet->from.id, push_id);
    return;
  }


  if (msgCode == Message::PushBlock) {
    if (!packet->message->has_push_id()) {
      LM_W(("Received a push block message without the push_id"));
      return;   // No sense to answer with this error
    }

    if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;   // No sense to answer with this error
    }

    if (packet->from.id == 0) {
      LM_W(("Received a push packet from a delilah_id = 0. Rejected" ));
      return;   // No sense to answer with this error
    }

    if (packet->buffer() == NULL) {
      LM_W(("Received a push block message without a buffer with data"));
      return;   // No sense to answer with this error
    }

    // Use push manager to deal with this type of message
    size_t push_id = packet->message->push_id();
    std::vector<std::string> queues;
    for (int i = 0; i < packet->message->queue_size(); i++) {
      queues.push_back(packet->message->queue(i));
    }

    push_manager->receive_push_block(packet->from.id, push_id, packet->buffer(), queues);
    return;
  }

  // --------------------------------------------------------------------
  // pop messages
  // --------------------------------------------------------------------

  if (msgCode == Message::PopQueue) {
    if (!packet->message->has_pop_queue()) {
      LM_W(("Received a pop message without pop information.Ignoring..."));
      return;
    }

    uint64 commit_id = packet->message->pop_queue().commit_id();
    size_t delilah_id = packet->from.id;
    std::string original_queue = packet->message->pop_queue().queue();
    std::string pop_queue = au::str(".%s_%lu_%s"
                                    , au::code64_str(delilah_id).c_str()
                                    , packet->message->delilah_component_id()
                                    , original_queue.c_str());

    // Generate packet with all queue content
    au::SharedPointer<Packet> p(new Packet(Message::PopQueueResponse));
    p->message->set_delilah_component_id(packet->message->delilah_component_id());
    p->to = packet->from;
    gpb::Queue *gpb_queue = p->message->mutable_pop_queue_response()->mutable_queue();
    gpb_queue->set_name(original_queue);
    gpb_queue->set_key_format("?");   // It is necessary to fill this fields
    gpb_queue->set_value_format("?");


    // Get a copy of the entire data model
    au::SharedPointer<gpb::Data> data = data_model->getCurrentModel();

    if (commit_id == static_cast<size_t>(-1)) {
      // Duplicate queue and link
      au::ErrorManager error;
      data_model->Commit("pop", au::str("add_queue_connection %s %s", original_queue.c_str(),
                                        pop_queue.c_str()), &error);

      if (error.IsActivated()) {
        LM_W(("Internal error with add_queue_connection command in pop request: %s", error.GetMessage().c_str()));
        return;
      }

      // If the queue really exist, return all its content to be popped
      gpb::Queue *queue = get_queue(data.shared_object(), original_queue);
      if (queue)
        gpb_queue->CopyFrom(*queue);
    } else {
      // Copy blocks newer than commit_id
      gpb::Queue *queue = get_queue(data.shared_object(), pop_queue);
      if (queue)
        for (int i = 0; i < queue->blocks_size(); i++) {
          if (queue->blocks(i).commit_id() > commit_id)
            gpb_queue->add_blocks()->CopyFrom(queue->blocks(i));
        }
    }

    // Send packet with information
    network_->Send(p);

    return;
  }

  // --------------------------------------------------------------------
  // Worker commands
  // --------------------------------------------------------------------

  if (msgCode == Message::WorkerCommand) {
    if (!packet->message->has_worker_command()) {
      LM_W(("Trying to run a WorkerCommand from a packet without that message"));
      return;
    }

    size_t delilah_id = packet->from.id;
    size_t delilah_component_id = packet->message->delilah_component_id();

    std::string worker_command_id = au::str("%s_%lu", au::code64_str(delilah_id).c_str(), delilah_component_id);

    WorkerCommand *workerCommand = new WorkerCommand(worker_command_id
                                                     , delilah_id
                                                     , delilah_component_id
                                                     , packet->message->worker_command()
                                                     );

    workerCommandManager->addWorkerCommand(workerCommand);
    return;
  }

  LM_W(("Received a message with type %s. Just ignoring...", messageCode(msgCode)));
}

// Receive notifications
void SamsonWorker::notify(engine::Notification *notification) {
  if (notification->isName("samson_worker_review")) {
    review();
    return;
  }

  if (notification->isName("notification_cluster_info_changed")) {
    // Change network setup to adapt to the new scenario
    samson::gpb::ClusterInfo *cluster_info;
    size_t cluster_info_version;
    worker_controller->GetClusterInfo(&cluster_info, &cluster_info_version);
    network_->set_cluster_information(cluster_info_version, cluster_info);

    // Reset worker manager to adapt to this new situation
    KVRanges ranges = worker_controller->GetMyKVRanges();
    task_manager->update_ranges(ranges);

    LM_W(("Cluster setup change. Assgined ranges %s", ranges.str().c_str()));

    // Reset this worker until all blocks are collected from disk /  rest of the cluster
    ResetWorker();
    return;
  }


  if (notification->isName(notification_samson_worker_review)) {
    CheckStatus();
    return;
  }

  if (notification->isName(notification_samson_worker_review2)) {
    // Get ids for all the blocks included in the model
    // and remove in the block manager old blocks not included
    std::set<size_t> block_ids = data_model->get_block_ids();
    stream::BlockManager::shared()->RemoveBlocksNotIncluded(block_ids);

    // Update my worker-node in ZK
    worker_controller->UpdateWorkerNode();

    return;
  }

  if (notification->isName("rest_operation_stream_manager")) {
    LM_X(1, ("Rest still not implemented"));
    /*
     *
     * au::network::RESTServiceCommand* command = (au::network::RESTServiceCommand*)notification->object();
     * if( command == NULL )
     * {
     * LM_W(("rest_connection notification without a command. This is probably an error..."));
     * return;
     * }
     *
     * // Sync-Engine implementation of the rest command
     * streamManager->process( command );             // Get this from the stream manager
     *
     * // Mark as finish and wake up thread to answer this connection
     * command->finish();
     */
    return;
  }


  if (notification->isName(notification_update_status)) {
    // Some ancient samson-0.6 useful Status information
    // Collect some information and print status...
    int num_processes = engine::ProcessManager::shared()->num_used_procesors();
    int max_processes = engine::ProcessManager::shared()->max_num_procesors();

    size_t used_memory = engine::MemoryManager::shared()->used_memory();
    size_t max_memory = engine::MemoryManager::shared()->memory();

    size_t disk_read_rate = (size_t) engine::DiskManager::shared()->get_rate_in();
    size_t disk_write_rate = (size_t) engine::DiskManager::shared()->get_rate_out();

    size_t network_read_rate = (size_t)network_->get_rate_in();
    size_t network_write_rate = (size_t)network_->get_rate_out();


    LM_M(("Status (%s) [ P %s M %s D_in %s D_out %s N_in %s N_out %s ]"
          , au::str_time(cronometer.seconds()).c_str()
          , au::str_percentage(num_processes, max_processes).c_str()
          , au::str_percentage(used_memory, max_memory).c_str()
          , au::str(disk_read_rate, "Bs").c_str()
          , au::str(disk_write_rate, "Bs").c_str()
          , au::str(network_read_rate, "Bs").c_str()
          , au::str(network_write_rate, "Bs").c_str()
          ));
  } else if (notification->isName(notification_samson_worker_send_trace)) {
    std::string message = notification->environment().get("message", "No message coming with trace-notification");
    std::string context = notification->environment().get("context", "?");
    std::string type    = notification->environment().get("type", "message");

    sendTrace(type, context, message);
  } else if (notification->isName(notification_samson_worker_take_sample)) {
    // Take samples every second to check current status
    samson_worker_samples.take_samples();
  } else {
    LM_W(("SamsonWorker received an unexpected notification %s. Ignoring...", notification->getDescription().c_str()));
  }
}

std::string getFormatedElement(std::string name, std::string value, std::string& format) {
  std::ostringstream output;

  if (format == "xml")
    au::xml_simple(output, name, value); else if (format == "json")
    au::json_simple(output, name, value); else if (format == "html")
    output << "<h1>" << name << "</h1>" << value; else
    output << name << ":\n" << value; return output.str();
}

std::string getFormatedError(std::string message, std::string& format) {
  return getFormatedElement("error", message, format);
}

void SamsonWorker::process_delilah_command(std::string delilah_command,
                                           au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Create client if not created
  if (!delilah)
    LM_X(1, ("Internal error"));  // Send the command
  LM_T(LmtDelilahCommand, ("Sending delilah command: '%s'", delilah_command.c_str()));
  size_t command_id = delilah->sendWorkerCommand(delilah_command);

  // Wait for the command to finish
  {
    au::Cronometer c;
    while (delilah->isActive(command_id)) {
      usleep(10000);
      if (c.seconds() > 2) {
        command->AppendFormatedError(500, au::str("Timeout awaiting response from REST client (task %lu)", command_id));
        LM_E(("Timeout awaiting response from REST client"));
        return;
      }
    }
  }

  // Recover information
  WorkerCommandDelilahComponent *component = (WorkerCommandDelilahComponent *)delilah->getComponent(command_id);
  if (!component) {
    command->AppendFormatedError(500, "Internal error recovering answer from REST client");
    LM_E(("Internal error recovering answer from REST client"));
    return;
  }

  // Recover table from component
  au::tables::Table *table = component->getMainTable();

  if (!table) {
    // command->AppendFormatedError(500, "No content in answer from REST client" );
    LM_E(("No content in answer from REST client"));
    return;
  }

  std::string output;

  LM_T(LmtRest, ("appending delilah output to command: '%s'", table->str().c_str()));

  if (command->format() == "xml")
    command->Append(table->str_xml());
  else if (command->format() == "json")
    command->Append(table->str_json());
  else if (command->format() == "html")
    command->Append(table->str_html());
  else
    command->Append(table->str());     // Default non-format
  delete table;
}

#define TF(b) ((b == true) ? "true" : "false")
void SamsonWorker::process_logging(au::SharedPointer<au::network::RESTServiceCommand> command) {
  std::ostringstream logdata;
  std::string logCommand  = "";
  std::string sub         = "";
  std::string arg         = "";

  command->set_http_state(200);

  if (command->path_components().size() > 2)
    logCommand = command->path_components()[2]; if (command->path_components().size() > 3)
    sub = command->path_components()[3]; if (command->path_components().size() > 4)
    arg = command->path_components()[4];  //
   // Treat all possible errors
  //

  if (logCommand == "") {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("no logging subcommand"));
  } else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "traces") &&
             (logCommand != "verbose") &&
             (logCommand != "debug"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad logging command: '%s'", logCommand.c_str()));
  } else if (((logCommand == "reads") || (logCommand == "writes") ||
              (logCommand == "debug")) && (sub != "on") && (sub != "off"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off")) {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub == "set") && (arg != "0") && (arg != "1") && (arg != "2") &&
             (arg != "3") && (arg != "4") &&
             (arg != "5"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad logging argument for 'verbose': %s", arg.c_str()));
  } else if ((logCommand == "traces") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove") &&
             (sub != "off") &&
             (sub != ""))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad logging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "traces") && ((sub != "set") || (sub != "add") || (sub != "remove"))) {
    if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str())) {
      command->set_http_state(400);
      command->AppendFormatedElement("message",
                                     au::str("bad logging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
    }
  }



  //
  // Checking the VERB
  //
  std::string verb = command->command();
  std::string path = logCommand;

  if (sub != "") path += '/' + sub; if ((path == "debug/on")      && (verb == "POST")) {
    ;
  } else if ((path == "debug/off")     && (verb == "POST")) {
    ;
  } else if ((path == "reads/on")      && (verb == "POST")) {
    ;
  } else if ((path == "reads/off")     && (verb == "POST")) {
    ;
  } else if ((path == "writes/on")     && (verb == "POST")) {
    ;
  } else if ((path == "writes/off")    && (verb == "POST")) {
    ;
  } else if ((path == "traces")        && (verb == "GET")) {
    ;
  } else if ((path == "traces/off")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/get")    && (verb == "GET")) {
    ;
  } else if ((path == "traces/set")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/add")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/remove") && (verb == "DELETE")) {
    ;
  } else if ((path == "verbose")       && (verb == "GET")) {
    ;
  } else if ((path == "verbose/off")   && (verb == "POST")) {
    ;
  } else if ((path == "verbose/set")   && (verb == "POST")) {
    ;
  } else {
    command->set_http_state(404);
    command->AppendFormatedElement("error", "BAD VERB");
    return;
  }

  if (command->http_state() != 200)
    return;

  //
  // Treat the request
  //
  if (logCommand == "reads") {
    if (sub == "on") {
      process_delilah_command("wreads on", command);
      command->AppendFormatedElement("reads", au::str("reads turned ON"));
      LM_F(("Turning on READS for entire cluster"));
    } else if (sub == "off") {
      process_delilah_command("wreads off", command);
      command->AppendFormatedElement("reads", au::str("reads turned OFF"));
      LM_F(("Turning off READS for entire cluster"));
    }
  } else if (logCommand == "writes") {
    if (sub == "on") {
      process_delilah_command("wwrites on", command);
      command->AppendFormatedElement("writes", au::str("writes turned ON"));
      LM_F(("Turning on WRITES for entire cluster"));
    } else if (sub == "off") {
      process_delilah_command("wwrites off", command);
      command->AppendFormatedElement("writes", au::str("writes turned OFF"));
      LM_F(("Turning off WRITES for entire cluster"));
    }
  } else if (logCommand == "debug") {
    if (sub == "on") {
      process_delilah_command("wdebug on", command);
      command->AppendFormatedElement("debug", au::str("debug turned ON"));
      LM_F(("Turning on DEBUG for entire cluster"));
      LM_D(("Debug should be ON"));
    } else if (sub == "off") {
      process_delilah_command("wdebug off", command);
      command->AppendFormatedElement("debug", au::str("debug turned OFF"));
      LM_F(("Turning off DEBUG for entire cluster"));
      LM_D(("Debug should be OFF"));
    }
  } else if (logCommand == "verbose") {  // /samson/logging/verbose
    if (sub == "")
      sub = "get"; if ((sub == "set") && (arg == "0"))
      sub = "off"; if (sub == "get") {
      process_delilah_command("wverbose get", command);
    } else if ((sub == "off") || (sub == "0")) {
      process_delilah_command("wverbose off", command);
      command->AppendFormatedElement("verbose", au::str("verbose mode turned OFF"));
      LM_F(("Turning off VERBOSE for entire cluster"));
      LM_V(("VERBOSE should be OFF"));
    } else {
      char delilahCommand[64];

      snprintf(delilahCommand, sizeof(delilahCommand), "wverbose %s", arg.c_str());
      process_delilah_command(delilahCommand, command);
      command->AppendFormatedElement("verbose", au::str("verbose levels upto %s SET", arg.c_str()));
      LM_F(("Setting VERBOSE level to %s for entire cluster", arg.c_str()));
      LM_V(("VERBOSE level 1"));
      LM_V2(("VERBOSE level 2"));
      LM_V3(("VERBOSE level 3"));
      LM_V4(("VERBOSE level 4"));
      LM_V5(("VERBOSE level 5"));
    }
  } else if (logCommand == "traces") {
    if (sub == "")
      sub = "get"; if (sub == "set") {
      char delilahCommand[64];

      snprintf(delilahCommand, sizeof(delilahCommand), "wtrace set %s",  arg.c_str());
      process_delilah_command(delilahCommand, command);
      command->AppendFormatedElement("trace", au::str("trace level: %s", arg.c_str()));
      LM_F(("Setting TRACE levels to '%s'", arg.c_str()));
    } else if (sub == "get") {  // /samson/logging/trace/get
      char delilahCommand[64];

      snprintf(delilahCommand, sizeof(delilahCommand), "wtrace get");
      process_delilah_command(delilahCommand, command);
      // What is pushed back to the REST request?
    } else if (sub == "off") {  // /samson/logging/trace/off
      process_delilah_command("wtrace off", command);
      command->AppendFormatedElement("trace", au::str("all trace levels turned off"));
      LM_F(("Resetting TRACE levels to 'NADA DE NADA'"));
    } else if (sub == "add") {  // /samson/logging/trace/add
      char delilahCommand[64];

      snprintf(delilahCommand, sizeof(delilahCommand), "wtrace add %s", arg.c_str());
      process_delilah_command(delilahCommand, command);
      command->AppendFormatedElement("trace", au::str("added level(s) %s", arg.c_str()));
      LM_F(("Adding TRACE levels '%s'", arg.c_str()));
    } else if (sub == "remove") {   // /samson/logging/trace/remove
      char delilahCommand[64];

      snprintf(delilahCommand, sizeof(delilahCommand), "wtrace remove %s", arg.c_str());
      process_delilah_command(delilahCommand, command);
      command->AppendFormatedElement("trace", au::str("removed level(s) %s", arg.c_str()));
      LM_F(("Removing TRACE levels '%s'", arg.c_str()));
    }
  }
}

#if 0
// ------------------------------------------------------------
//
// Future common check for path and verb in logging/ilogging
//

static bool restTraceCheck(au::network::RESTServiceCommand *command) {
  std::string arg = command->path_components[4];

  if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str()))
    return false;



  return true;
}

static bool restVerboseCheck(au::network::RESTServiceCommand *command) {
  std::string arg = command->path_components[4];

  if ((arg == "0") || (arg == "1") || (arg == "2") || (arg == "3") || (arg == "4") || (arg == "5"))
    return true;




  return false;
}

typedef bool (*RestCheckFunc)(au::network::RESTServiceCommand *command);
typedef struct RestCheck {
  std::string verb;
  std::string path;
  int components;                // Not counting '/samson/XXX' ,,,
  RestCheckFunc checkFunc;
} RestCheck;

RestCheck restCheck[] =
{
  { "POST",   "debug/on",      2,        NULL                                                                                                  },
  { "POST",   "debug/off",     2,        NULL                                                                                                  },
  { "POST",   "reads/on",      2,        NULL                                                                                                  },
  { "POST",   "reads/off",     2,        NULL                                                                                                  },
  { "POST",   "writes/on",     2,        NULL                                                                                                  },
  { "POST",   "writes/off",    2,        NULL                                                                                                  },
  { "GET",    "traces",        1,        NULL                                                                                                  },
  { "POST",   "traces/off",    2,        NULL                                                                                                  },
  { "POST",   "traces/set",    3,        restTraceCheck                                                                                        },
  { "POST",   "traces/add",    3,        restTraceCheck                                                                                        },
  { "DELETE", "traces/remove", 3,        restTraceCheck                                                                                        },
  { "GET",    "verbose",       1,        NULL                                                                                                  },
  { "POST",   "verbose/off",   2,        NULL                                                                                                  },
  { "POST",   "verbose/set",   3,        restVerboseCheck                                                                                      },
};

static bool process_logging_check(au::network::RESTServiceCommand *command) {
  std::string verb = command->command;
  std::string path = command->path_components[2];
  bool ok   = false;

  if (command->path_components[3] != "")
    path += '/' + command->path_components[3];

  for (unsigned int ix = 0; ix < sizeof(restCheck) / sizeof(restCheck[0]); ix++) {
    if ((path == restCheck[ix].path) && (verb == restCheck[ix].verb) &&
        (command->path_components.size() == restCheck[ix].components + 2))
    {
      // A match - now just lets see if the argument is OK also ...
      if (restCheck[ix].check != NULL)
        return restCheck[ix].check(command);



      return true;
    }
  }

  return ok;
}

#endif  // if 0

void SamsonWorker::process_ilogging(au::SharedPointer<au::network::RESTServiceCommand> command) {
  std::ostringstream logdata;
  std::string logCommand  = "";
  std::string sub         = "";
  std::string arg         = "";

  command->set_http_state(200);

  if (command->path_components().size() > 2)
    logCommand = command->path_components()[2]; if (command->path_components().size() > 3)
    sub = command->path_components()[3]; if (command->path_components().size() > 4)
    arg = command->path_components()[4];  //
   // Treat all possible errors
  //

  if (logCommand == "") {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("no ilogging subcommand"));
  } else if ((logCommand != "reads") && (logCommand != "writes") && (logCommand != "traces") &&
             (logCommand != "verbose") &&
             (logCommand != "debug"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad ilogging command: '%s'", logCommand.c_str()));
  } else if (((logCommand == "reads") || (logCommand == "writes") ||
              (logCommand == "debug")) && (sub != "on") && (sub != "off"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub != "get") && (sub != "set") && (sub != "off") && (sub != "")) {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "verbose") && (sub == "set") && (arg != "0") && (arg != "1") && (arg != "2") &&
             (arg != "3") && (arg != "4") &&
             (arg != "5"))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message", au::str("bad ilogging argument for 'verbose': %s", arg.c_str()));
  } else if ((logCommand == "traces") && (sub != "get") && (sub != "set") && (sub != "add") && (sub != "remove") &&
             (sub != "off") &&
             (sub != ""))
  {
    command->set_http_state(400);
    command->AppendFormatedElement("message",
                                   au::str("bad ilogging subcommand for '%s': %s", logCommand.c_str(), sub.c_str()));
  } else if ((logCommand == "traces") && ((sub != "set") || (sub != "add") || (sub != "remove"))) {
    if (strspn(arg.c_str(), "0123456789-,") != strlen(arg.c_str())) {
      command->set_http_state(400);
      command->AppendFormatedElement("message",
                                     au::str("bad ilogging parameter '%s' for 'trace/%s'", arg.c_str(), sub.c_str()));
    }
  }


  //
  // Checking the VERB
  //
  std::string verb = command->command();
  std::string path = logCommand;

  if (sub != "") path += '/' + sub; if ((path == "debug/on")      && (verb == "POST")) {
    ;
  } else if ((path == "debug/off")     && (verb == "POST")) {
    ;
  } else if ((path == "reads/on")      && (verb == "POST")) {
    ;
  } else if ((path == "reads/off")     && (verb == "POST")) {
    ;
  } else if ((path == "writes/on")     && (verb == "POST")) {
    ;
  } else if ((path == "writes/off")    && (verb == "POST")) {
    ;
  } else if ((path == "traces")        && (verb == "GET")) {
    ;
  } else if ((path == "traces/off")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/get")    && (verb == "GET")) {
    ;
  } else if ((path == "traces/set")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/add")    && (verb == "POST")) {
    ;
  } else if ((path == "traces/remove") && (verb == "DELETE")) {
    ;
  } else if ((path == "verbose")       && (verb == "GET")) {
    ;
  } else if ((path == "verbose/off")   && (verb == "POST")) {
    ;
  } else if ((path == "verbose/set")   && (verb == "POST")) {
    ;
  } else {
    command->set_http_state(404);
    command->AppendFormatedElement("error", "BAD VERB");
    return;
  }

  if (command->http_state() != 200)
    return;

  //
  // Treat the request
  //
  if (logCommand == "reads") {
    if (sub == "on") {
      lmReads  = true;
      command->AppendFormatedElement("reads", au::str("reads turned ON"));
      LM_F(("Turned on READS for this node only"));
    } else if (sub == "off") {
      lmReads  = false;
      command->AppendFormatedElement("reads", au::str("reads turned OFF"));
      LM_F(("Turned off READS for this node only"));
    }
  } else if (logCommand == "writes") {
    if (sub == "on") {
      lmWrites  = true;
      command->AppendFormatedElement("writes", au::str("writes turned ON"));
      LM_F(("Turned on WRITES for this node only"));
    } else if (sub == "off") {
      lmWrites  = false;
      command->AppendFormatedElement("writes", au::str("writes turned OFF"));
      LM_F(("Turned off WRITES for this node only"));
    }
  } else if (logCommand == "debug") {
    if (sub == "on") {
      lmDebug  = true;
      command->AppendFormatedElement("debug", au::str("debug turned ON"));
      LM_F(("Turned on DEBUG for this node only"));
      LM_D(("Turned on DEBUG for this node only"));
    } else if (sub == "off") {
      lmDebug  = false;
      command->AppendFormatedElement("debug", au::str("debug turned OFF"));
      LM_F(("Turned off DEBUG for this node only"));
      LM_D(("This line should not be seen ..."));
    }
  } else if (logCommand == "verbose") {  // /samson/ilogging/verbose
    if (sub == "")
      sub = "get"; if ((sub == "set") && (arg == "0"))
      sub = "off"; if (sub == "get") {
      int vLevel;

      if (lmVerbose5 == true) vLevel = 5; else if (lmVerbose4 == true) vLevel = 4; else if (lmVerbose3 ==
                                                                                            true)
        vLevel = 3;
      else if (lmVerbose2 == true) vLevel = 2; else if (lmVerbose  == true) vLevel = 1; else vLevel = 0;
      command->AppendFormatedElement(
        "verbose", au::str("verbosity level: %d", vLevel));
    } else {
      // Turn all verbose levels OFF
      lmVerbose  = false;
      lmVerbose2 = false;
      lmVerbose3 = false;
      lmVerbose4 = false;
      lmVerbose5 = false;

      if (sub == "off")
        arg = "0"; int verboseLevel = arg[0] - '0';

      // Turn on the desired verbose levels
      switch (verboseLevel) {
        case 5: lmVerbose5 = true;
        case 4: lmVerbose4 = true;
        case 3: lmVerbose3 = true;
        case 2: lmVerbose2 = true;
        case 1: lmVerbose  = true;
      }

      command->AppendFormatedElement("verbose", au::str("verbosity level: %d", verboseLevel));
      LM_F(("New verbose level for this node only: %d", verboseLevel));
    }
  } else if (logCommand == "traces") {
    if (sub == "")
      sub = "get"; if (sub == "set") {
      lmTraceSet((char *)arg.c_str());
      command->AppendFormatedElement("trace", au::str("trace level: %s", arg.c_str()));
      LM_F(("Set trace levels to '%s' for this node only", arg.c_str()));
    } else if (sub == "get") {  // /samson/ilogging/trace/get
      char traceLevels[1024];
      lmTraceGet(traceLevels);

      command->AppendFormatedElement("trace", au::str("trace level: %s", traceLevels));
    } else if (sub == "off") {  // /samson/ilogging/trace/off
      lmTraceSet(NULL);
      command->AppendFormatedElement("trace", au::str("all trace levels turned off"));
      LM_F(("Turned off trace levels for this node only"));
    } else if (sub == "add") {  // /samson/ilogging/trace/add
      lmTraceAdd((char *)arg.c_str());
      command->AppendFormatedElement("trace", au::str("added level(s) %s", arg.c_str()));
      LM_F(("Added trace levels '%s' for this node only", arg.c_str()));
    } else if (sub == "remove") {   // /samson/ilogging/trace/remove
      lmTraceSub((char *)arg.c_str());
      command->AppendFormatedElement("trace", au::str("removed level(s) %s", arg.c_str()));
      LM_F(("Removed trace levels '%s' for this node only", arg.c_str()));
    }
  }
}

void SamsonWorker::process(au::SharedPointer<au::network::RESTServiceCommand> command) {
  // Default format
  if (command->format() == "") {
    command->set_format("xml");  // Begin data for each format
  }
  // ---------------------------------------------------
  if (command->format() == "xml") {
    command->Append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    command->Append("<!-- SAMSON Rest interface -->\n");
    command->Append("<samson>\n");
  } else if (command->format() == "html") {
    command->Append("<html><body>");
  } else if (command->format() == "json") {
    /*
     * {
     * command->Append("{");
     * }
     */

    // Internal process of the command
    process_intern(command);
  }

  // Close data content
  // ---------------------------------------------------
  if (command->format() == "xml") {
    command->Append("\n</samson>\n");
  } else if (command->format() == "html") {
    command->Append("</body></html>");  /*
                                         * else if( command->format == "json" )
                                         * command->Append("}");
                                         */
  }
}

/* ****************************************************************************
 *
 * clusterNodeAdd -
 */

void SamsonWorker::process_intern(au::SharedPointer<au::network::RESTServiceCommand> command) {
  std::string main_command  = command->path_components()[1];
  std::string path          = "";
  std::string verb          = command->command();
  unsigned int components    = command->path_components().size();

  for (unsigned int ix = 0; ix < components; ix++) {
    path += std::string("/") + command->path_components()[ix];
  }
  LM_T(LmtRest, ("Incoming REST request: %s '%s'", verb.c_str(), path.c_str()));

  //
  // Quick sanity check
  //

  if (components < 2) {
    command->AppendFormatedError(400, "Only /samson/option paths are valid");
    return;
  }

  if (command->path_components()[0] != "samson") {
    command->AppendFormatedError(400, "Only /samson/option paths are valid");
    return;
  }

  //
  // Treat the message
  //

  if ((path == "/samson/version") && (verb == "GET")) {
    command->AppendFormatedElement("version", au::str("SAMSON v %s", SAMSON_VERSION));
  } else if ((path == "/samson/status") && (verb == "GET")) {
    /*
     * std::ostringstream data;
     * network->getInfo(data, "cluster", command->format);
     * command->Append( data.str() );
     */
  } else if ((path == "/samson/cluster") && (verb == "GET")) {
    /*
     * std::ostringstream data;
     * network->getInfo(data, "cluster", command->format);
     * command->Append( data.str() );
     */
  } else if (main_command == "ilogging") {
    process_ilogging(command);
  } else if (main_command == "logging") {
    process_logging(command);
  } else if (main_command == "node") {
    process_node(command);
  } else if ((path == "/samson/queues") && (verb == "GET")) {
    process_delilah_command("ls -group name -sort name", command);
  } else if ((path == "/samson/queues_rates") && (verb == "GET")) {
    process_delilah_command("ls -group name -sort name -rates", command);
  } else if (main_command == "queues") {
    char delilahCommand[256];

    if ((components == 3) && (verb == "GET")) {
      snprintf(delilahCommand, sizeof(delilahCommand), "ls %s", command->path_components()[2].c_str());
      process_delilah_command(delilahCommand, command);
    } else if ((components == 4) && (command->path_components()[3] == "delete") && (verb == "DELETE")) {
      snprintf(delilahCommand, sizeof(delilahCommand), "rm %s", command->path_components()[2].c_str());
      process_delilah_command(delilahCommand, command);
    } else if ((components == 5) && (command->path_components()[3] == "state") && (verb == "GET")) {
      // This is done synchronous with engine

      // Add element to the engine
      engine::Notification *notification = new engine::Notification("rest_operation_stream_manager");
      // notification->dictionary().Set("command", command );
      LM_X(1, ("Unimplemented"));

      engine::Engine::shared()->notify(notification);

      // Wait until it is resolved
      command->WaitUntilFinished();
    } else {
      command->AppendFormatedError(404, au::str("Bad VERB or PATH"));
    }
  } else if (main_command == "command") {  /* /samson/command */
    std::string delilah_command = "";

    if (command->command() != "GET") {
      command->AppendFormatedError(404, au::str("bad VERB for command"));
      return;
    }

    if (components == 2)
      delilah_command = "ls";   // 'ls' is the default command
    else if (components == 3)
      delilah_command = command->path_components()[2];

    if (delilah_command != "")
      process_delilah_command(delilah_command, command); else
      command->AppendFormatedError(404, au::str("bad path for command"));
  } else if ((path == "/samson/modules") && (verb == "GET")) {
    process_delilah_command("ls_modules -group name", command);
  } else if ((main_command == "modules") && (components == 3) && (verb == "GET")) {
    char delilahCommand[256];

    snprintf(delilahCommand, sizeof(delilahCommand), "ls_modules %s -group name", command->path_components()[2].c_str());
    process_delilah_command(delilahCommand, command);
  } else if (main_command == "stream_operations") {
    if (command->command() != "GET") {
      command->AppendFormatedError(404, au::str("bad VERB for command"));
      return;
    }
    std::string filter;
    if (components == 3)
      filter = command->path_components()[2]; process_delilah_command(au::str("ls_stream_operations %s -group name",
                                                                              filter.c_str()), command);
  } else if (main_command == "workers") {
    if (command->command() != "GET") {
      command->AppendFormatedError(404, au::str("bad VERB for command"));
      return;
    }
    process_delilah_command("ls_workers", command);
  } else if (main_command == "connections") {
    if (command->command() != "GET") {
      command->AppendFormatedError(404, au::str("bad VERB for command"));
      return;
    }
    process_delilah_command("ls_connections -group name", command);
  } else if (main_command == "stream_operations_instances") {
    if (command->command() != "GET") {
      command->AppendFormatedError(404, au::str("bad VERB for command"));
      return;
    }
    process_delilah_command("ps_tasks", command);
  } else if (main_command == "operations") {  /* /samson/operations */
    char delilahCommand[256];

    if ((command->command() == "GET") && (components == 2)) {
      snprintf(delilahCommand, sizeof(delilahCommand), "ls_operations -group name");
      process_delilah_command(delilahCommand, command);
    } else if ((command->command() == "GET") && (components == 3)) {
      snprintf(delilahCommand, sizeof(delilahCommand), "ls_operations %s -group name",
               command->path_components()[2].c_str());
      process_delilah_command(delilahCommand, command);
    } else if ((command->command() == "PUT") && (components == 3)) {
      // Need to parse the XML here ...
      command->AppendFormatedError(400, au::str("Not Implemented"));
    } else if ((command->command() == "DELETE") && (components == 4) && (command->path_components()[2] == "delete")) {
      // snprintf(delilahCommand, sizeof(delilahCommand), "rm_stream_operation %s", command->path_components[3].c_str());
      // process_delilah_command(delilahCommand, command);
      command->AppendFormatedError(400, au::str("Not Implemented"));
    } else {
      command->AppendFormatedError(404, au::str("bad path/verb"));
    }
  }
#if 0
  else if (( main_command == "state" ) || ( main_command == "queue" )) {
    /* /samson/state/queue/key */
    if (components < 4)
      command->AppendFormatedError(400, "Only /samson/state/queue/key paths are valid");
    else
      streamManager->process(command);                 // Get this from the stream manager
  } else if (main_command == "data_test") {
    command->AppendFormatedElement("data_size", au::str("%lu", command->data_size));

    if (command->data_size == 0) {
      command->AppendFormatedElement("Data", "No data provided in the REST request");
    } else {
      // Return with provided data
      std::string data;
      data.append(command->data, command->data_size);
      command->AppendFormatedElement("data", data);
    }
  }
#endif
  else {
    command->AppendFormatedError(404, au::str("Bad VERB or PATH"));
  }
}

void SamsonWorker::process_node(au::SharedPointer<au::network::RESTServiceCommand> command) {
  if (command->format() != "json") {
    command->AppendFormatedError(400, "Only json format is supported in samson/node/*");
    return;
  }

  if (command->path_components().size() <= 2) {
    command->AppendFormatedError(400, "Bad path. Supported: samson/node/general.json");
    return;
  }

  //  /samson/status/
  std::string sub_command = command->path_components()[2];

  if (sub_command == "general") {
    command->Append(samson_worker_samples.getJson());
    return;
  } else {
    command->AppendFormatedError(400, "Bad path. Supported: samson/node/general.json");
    return;
  }
}

// Get information for monitoring
void SamsonWorker::getInfo(std::ostringstream& output) {
  // Engine
  // engine::Engine::shared()->getInfo( output );

  // Engine system
  // samson::getInfoEngineSystem(output, network);

  // Modules manager
  // ModulesManager::shared()->getInfo( output );

  // Block manager
  // stream::BlockManager::shared()->getInfo( output );

  // Queues manager information
  // streamManager->getInfo(output);


  // WorkerCommandManager
  // workerCommandManager->getInfo(output);

  // Network
  // network->getInfo( output , "main" );
}

void SamsonWorker::autoComplete(au::ConsoleAutoComplete *info) {
  if (info->completingFirstWord()) {
    info->add("quit");
    info->add("exit");
    info->add("threads");
    info->add("cluster");
    info->add("show_logs");
    info->add("hide_logs");
    info->add("show_engine_statistics");
    info->add("show_engine_last_items");
    info->add("show_engine_elements");
    info->add("show_engine_current_element");
  }
}

void SamsonWorker::evalCommand(std::string command) {
  au::CommandLine cmdLine;

  cmdLine.parse(command);

  if (cmdLine.get_num_arguments() == 0)
    return;

  std::string main_command = cmdLine.get_argument(0);

  if (main_command == "quit")
    quitConsole(); if (main_command == "exit")
    quitConsole(); if (main_command == "threads") {
    const au::ThreadManager& r = *au::ThreadManager::shared();
    writeOnConsole(au::S(r));
  }

  if (main_command == "show_logs") {
    au::add_log_plugin(this);
    writeOnConsole("OK\n");
  }
  if (main_command == "hide_logs") {
    au::remove_log_plugin(this);
    writeOnConsole("OK\n");
  }

  if (main_command == "show_engine_current_element") {
    writeOnConsole(engine::Engine::shared()->get_activity_monitor()->GetCurrentActivity() + "\n");
    return;
  }

  if (main_command == "show_engine_statistics") {
    writeOnConsole(engine::Engine::shared()->get_activity_monitor()->GetElementsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_last_items") {
    writeOnConsole(engine::Engine::shared()->get_activity_monitor()->GetLastItemsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_elements") {
    writeOnConsole(engine::Engine::shared()->getTableOfEngineElements() + "\n");
    return;
  }


  // More command to check what is going on inside a worker
}

std::string SamsonWorker::getPrompt() {
  return "SamsonWorker> ";
}

void SamsonWorker::sendTrace(std::string type, std::string context, std::string message) {
  // Send message to all delilahs
  std::vector<size_t> delilahs = network_->getDelilahIds();

  for (size_t i = 0; i < delilahs.size(); i++) {
    PacketPointer p(new Packet(Message::Alert));

    gpb::Alert *alert = p->message->mutable_alert();
    alert->set_type(type);
    alert->set_context(context);
    alert->set_text(message);

    p->message->set_delilah_component_id((size_t)-1);     // This message do not belong to the operation executing it

    // Direction of this paket
    p->to.node_type = DelilahNode;
    p->to.id = delilahs[i];

    // Send packet
    network_->Send(p);
  }
}

gpb::Collection *SamsonWorker::getWorkerCollection(const Visualization& visualization) {
  gpb::Collection *collection = new gpb::Collection();

  collection->set_name("workers");

  gpb::CollectionRecord *record = collection->add_record();

  if (visualization.get_flag("engine")) {
    size_t num_elements = engine::Engine::shared()->getNumElementsInEngineStack();
    double waiting_time = engine::Engine::shared()->getMaxWaitingTimeInEngineStack();
    size_t num_buffers = engine::MemoryManager::shared()->num_buffers();
    ::samson::add(record, "#buffers in memory", num_buffers, "f=uint64,sum");
    ::samson::add(record, "#elements in engine", num_elements, "f=uint64,sum");
    ::samson::add(record, "Max waiting time", waiting_time, "f=double,different");
  } else if (visualization.get_flag("disk")) {
    // Disk activiry

    ::samson::add(record, "Disk in B/s", engine::DiskManager::shared()->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "Disk out B/s", engine::DiskManager::shared()->get_rate_out(), "f=uint64,sum");

    double op_in = engine::DiskManager::shared()->get_rate_operations_in();
    double op_out = engine::DiskManager::shared()->get_rate_operations_out();

    ::samson::add(record, "Disk in Ops/s", op_in, "f=double , sum");
    ::samson::add(record, "Disk out Ops/s", op_out, "f=double,sum");


    double on_time = engine::DiskManager::shared()->on_off_monitor.get_on_time();
    double off_time = engine::DiskManager::shared()->on_off_monitor.get_off_time();
    ::samson::add(record, "On time", on_time, "f=double,differet");
    ::samson::add(record, "Off time", off_time, "f=double,differet");

    ::samson::add(record, "BM writing",  stream::BlockManager::shared()->get_scheduled_write_size(), "f=uint64,sum");
    ::samson::add(record, "BM reading",  stream::BlockManager::shared()->get_scheduled_read_size(), "f=uint64,sum");

    double usage =  engine::DiskManager::shared()->get_on_off_activity();
    ::samson::add(record, "Disk usage", au::str_percentage(usage), "differet");
  } else {
    // Add ready flag
    ::samson::add(record, "Status", status_message_, "different");

    ::samson::add(record, "ZK", zoo_connection_->GetStatusString(), "different,left");

    ::samson::add(record, "Mem used", engine::MemoryManager::shared()->used_memory(), "f=uint64,sum");
    ::samson::add(record, "Mem total", engine::MemoryManager::shared()->memory(), "f=uint64,sum");

    ::samson::add(record, "Cores used", engine::ProcessManager::shared()->num_used_procesors(), "f=uint64,sum");
    ::samson::add(record, "Cores total", engine::ProcessManager::shared()->max_num_procesors(), "f=uint64,sum");

    ::samson::add(record, "#Disk ops", engine::DiskManager::shared()->getNumOperations(), "f=uint64,sum");

    ::samson::add(record, "Disk in B/s", engine::DiskManager::shared()->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "Disk out B/s", engine::DiskManager::shared()->get_rate_out(), "f=uint64,sum");

    ::samson::add(record, "Net in B/s", network_->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "Net out B/s", network_->get_rate_out(), "f=uint64,sum");
  }

  return collection;
}

void SamsonWorkerSamples::take_samples() {
  int num_processes = engine::ProcessManager::shared()->num_used_procesors();
  int max_processes = engine::ProcessManager::shared()->max_num_procesors();

  size_t used_memory = engine::MemoryManager::shared()->used_memory();
  size_t max_memory = engine::MemoryManager::shared()->memory();

  size_t disk_read_rate = (size_t) engine::DiskManager::shared()->get_rate_in();
  size_t disk_write_rate = (size_t) engine::DiskManager::shared()->get_rate_out();

  size_t network_read_rate = (size_t)samsonWorker_->network_->get_rate_in();
  size_t network_write_rate = (size_t)samsonWorker_->network_->get_rate_out();

  cpu.push(100.0 *  (double)num_processes / (double)max_processes);
  memory.push(100.0 * (double)used_memory / (double)max_memory);

  disk_in.push(disk_read_rate / 1000000);
  disk_out.push(disk_write_rate / 1000000);

  net_in.push(network_read_rate / 1000000);
  net_out.push(network_write_rate / 1000000);
}

size_t SamsonWorker::get_new_block_id() {
  while (true) {
    std::string path_base = "/samson/blocks/b";
    std::string path = path_base;

    int rc = zoo_connection_->Create(path, ZOO_SEQUENCE | ZOO_EPHEMERAL);
    if (rc) {
      // Some error...
      LM_W(("Error creating node to get a new block id: %s", samson::zoo::str_error(rc).c_str()));
      continue;
    } else {
      // Created blocks
      size_t block_id = atoll(path.substr(path_base.length()).c_str());

      // Remove node
      rc = zoo_connection_->Remove(path);
      if (rc)
        LM_W(("Not possible to remove node at %s", path.c_str()));  // Check non zero id generated
      if (block_id == 0) {
        LM_W(("Wrong block_id generated"));
        continue;
      }

      return block_id;
    }
  }
}

void SamsonWorker::CheckStatus() {
  // Check zoo connection
  if (!zoo_connection_->IsConnected()) {
    LM_W(("ZK is disconnected.... reseting this worker to connect again"));
    Connect();
  }

  if (status_ready_to_process_)
    return;   // Nothing else to check?

   // Get a list of all the blocks I should have
  KVRanges hg_ranges = worker_controller->GetMyKVRanges();
  std::set<size_t> block_ids = data_model->get_my_block_ids(hg_ranges);

  // Check if all the blocks are contained ( otherwise ask for them )
  std::set<size_t> pending_block_ids = stream::BlockManager::shared()->GetPendingBlockIds(block_ids);

  if (pending_block_ids.size() == 0) {
    status_message_ = "ready";
    status_ready_to_process_ =  true;
  } else {
    status_message_ = au::S() << "Pending " << pending_block_ids.size() << " blocks";

    // Recover information from all the workers in the cluster
    distribution_blocks_manager->RequestBlocks(pending_block_ids);

    return;
  }
}
}

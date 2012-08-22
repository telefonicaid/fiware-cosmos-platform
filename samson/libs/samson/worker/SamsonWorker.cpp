

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
#include "au/log/LogToServer.h"
#include "au/network/RESTServiceCommand.h"
#include "au/string.h"                            // au::Format
#include "au/tables/pugixml.hpp"                  // pugixml
#include "au/time.h"                              // au::todayString

#include "engine/DiskManager.h"                   // Notifications
#include "engine/DiskOperation.h"                 // samson::DiskOperation
#include "engine/Engine.h"                        // engine::Engine
#include "engine/MemoryManager.h"                 // samson::SharedMemory
#include "engine/Notification.h"                  // engine::Notification
#include "engine/ProcessManager.h"                // engine::ProcessManager

#include "samson/common/KVRange.h"
#include "samson/common/Macros.h"                 // EXIT, ...
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/isolated/SharedMemoryManager.h"  // engine::SharedMemoryManager
#include "samson/module/samsonVersion.h"          // SAMSON_VERSION
#include "samson/network/Message.h"               // Message
#include "samson/network/NetworkInterface.h"      // NetworkInterface
#include "samson/network/Packet.h"                // samson::Packet
#include "samson/network/WorkerNetwork.h"


#include "samson/delilah/WorkerCommandDelilahComponent.h"
#include "samson/module/ModulesManager.h"         // samson::ModulesManager
#include "samson/network/NetworkInterface.h"      // samson::NetworkInterface
#include "samson/stream/Block.h"                  // samson::stream::Block
#include "samson/stream/BlockList.h"              // samson::stream::BlockList
#include "samson/stream/BlockManager.h"           // samson::stream::BlockManager
#include "samson/stream/WorkerTaskManager.h"
#include "samson/worker/DistributionBlocks.h"
#include "samson/worker/PushManager.h"
#include "samson/worker/SamsonWorkerRest.h"
#include "samson/worker/WorkerCommand.h"  // samson::stream::WorkerCommand

#include "samson/worker/SamsonWorker.h"  // Own interfce

#define notification_samson_worker_review  "notification_samson_worker_review"
#define notification_samson_worker_review2 "notification_samson_worker_review2"

extern int port;                         // Port where samsonWorker is started
extern int web_port;

namespace samson {
/* ****************************************************************************
 *
 * Constructor
 */

SamsonWorker::SamsonWorker(std::string zoo_host, int port, int web_port) {
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

  // Push manager
  push_manager = new samson::worker::PushManager(this);

  // Init worker command manager
  workerCommandManager = new WorkerCommandManager(this);
  LM_T(LmtCleanup, ("Created workerCommandManager: %p", workerCommandManager));

  // Init distribution block
  distribution_blocks_manager = new DistributionBlockManager(this);


  // Init REST service
  samson_worker_rest = new SamsonWorkerRest(this, web_port);

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

  if (worker_controller) {
    delete worker_controller;
  }
  if (data_model) {
    delete data_model;
  }
  if (network_) {
    delete network_;
  }
  if (samson_worker_rest) {
    delete samson_worker_rest;
  }
  if (task_manager) {
    delete task_manager;
  }
}

void SamsonWorker::review() {
  // Review distribution of blocks
  distribution_blocks_manager->Review();

  // Review task_manager if we are ready to process data
  if (status_ready_to_process_) {
    task_manager->review_stream_operations();
  }
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

  if (msgCode == Message::StatusReport) {
    // LM_M(("Recieved status report message from %s" , packet->from.str().c_str() ));
    return;
  }

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
      return;     // No sense to answer with this error
    } else if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;     // No sense to answer with this error
    }

    // Use push manager to deal with this type of message
    size_t push_id = packet->message->push_id();
    push_manager->receive_push_block_commit(packet->from.id, push_id);
    return;
  }


  if (msgCode == Message::PushBlock) {
    if (!packet->message->has_push_id()) {
      LM_W(("Received a push block message without the push_id"));
      return;     // No sense to answer with this error
    }

    if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;     // No sense to answer with this error
    }

    if (packet->from.id == 0) {
      LM_W(("Received a push packet from a delilah_id = 0. Rejected" ));
      return;     // No sense to answer with this error
    }

    if (packet->buffer() == NULL) {
      LM_W(("Received a push block message without a buffer with data"));
      return;     // No sense to answer with this error
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
    gpb_queue->set_key_format("?");     // It is necessary to fill this fields
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
      if (queue) {
        gpb_queue->CopyFrom(*queue);
      }
    } else {
      // Copy blocks newer than commit_id
      gpb::Queue *queue = get_queue(data.shared_object(), pop_queue);
      if (queue) {
        for (int i = 0; i < queue->blocks_size(); i++) {
          if (queue->blocks(i).commit_id() > commit_id) {
            gpb_queue->add_blocks()->CopyFrom(queue->blocks(i));
          }
        }
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
    std::string message = notification->environment().Get("message", "No message coming with trace-notification");
    std::string context = notification->environment().Get("context", "?");
    std::string type    = notification->environment().Get("type", "message");

    sendTrace(type, context, message);
  } else {
    LM_W(("SamsonWorker received an unexpected notification %s. Ignoring...", notification->getDescription().c_str()));
  }
}

std::string getFormatedElement(std::string name, std::string value, std::string& format) {
  std::ostringstream output;

  if (format == "xml") {
    au::xml_simple(output, name, value);
  } else if (format == "json") {
    au::json_simple(output, name, value);
  } else if (format == "html") {
    output << "<h1>" << name << "</h1>" << value;
  } else {
    output << name << ":\n" << value;
  } return output.str();
}

std::string getFormatedError(std::string message, std::string& format) {
  return getFormatedElement("error", message, format);
}

void SamsonWorker::Stop() {
  // Stop rest interface
  samson_worker_rest->Stop();
}

// Get information for monitoring
void SamsonWorker::getInfo(std::ostringstream& output) {
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

  cmdLine.Parse(command);

  if (cmdLine.get_num_arguments() == 0) {
    return;
  }

  std::string main_command = cmdLine.get_argument(0);

  if (main_command == "quit") {
    quitConsole();
  }
  if (main_command == "exit") {
    quitConsole();
  }
  if (main_command == "threads") {
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

    p->message->set_delilah_component_id((size_t)-1);       // This message do not belong to the operation executing it

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


    double on_time = engine::DiskManager::shared()->on_off_monitor.on_time();
    double off_time = engine::DiskManager::shared()->on_off_monitor.off_time();
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
      if (rc) {
        LM_W(("Not possible to remove node at %s", path.c_str()));    // Check non zero id generated
      }
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

  if (status_ready_to_process_) {
    return;     // Nothing else to check?
  }
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



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
#include "au/file.h"
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
#include "samson/common/NotificationMessages.h"
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/gpb_operations.h"
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
#include "samson/worker/SamsonWorkerRest.h"
#include "samson/worker/WorkerCommand.h"  // samson::stream::WorkerCommand

#include "samson/worker/SamsonWorker.h"  // Own interfce

namespace samson {
/* ****************************************************************************
 *
 * Constructor
 */

SamsonWorker::SamsonWorker(std::string zoo_host, int port, int web_port) {
  zoo_host_ = zoo_host;
  port_ = port;
  web_port_ = web_port;

  last_modules_version_ = 0;

  // Random initialization
  srand(time(NULL));

  // Components
  worker_block_manager_.Reset(new WorkerBlockManager(this));
  workerCommandManager_ = new WorkerCommandManager(this);
  samson_worker_rest_ = new SamsonWorkerRest(this, web_port_);
  task_manager_ =  new stream::WorkerTaskManager(this);
  // network_ will be properly initialized later
  network_ = NULL;

  // Initial state of this worker ( unconnected )
  state_ = unconnected;
  state_message_ = "Worker starting...";

  listen(notification_samson_worker_send_message);              // Listen this notification to send traces
  listen(notification_update_status);                         // Notification to update state
  listen("notification_cluster_info_changed_in_worker");      // Cluster setup changes
  listen("samson_worker_review");                             // Review notification ( in this review all connections are stablished )
  listen(notification_packet_received);

  samson::SamsonSetup *samson_setup = au::Singleton<samson::SamsonSetup>::shared();

  engine::notify(notification_update_status, samson_setup->getInt("general.update_status_period"));
  engine::notify(notification_samson_worker_check_finish_tasks, samson_setup->getInt("worker.period_check_finish_tasks"));
  engine::notify("samson_worker_review", 2);
  engine::notify_extra("samson_worker_review");


  // Notification to review push manager
  engine::Engine::shared()->notify(new engine::Notification(notification_worker_block_manager_review), 5);
}

void SamsonWorker::Review() {
  // If zoo connection is not valid, come back to unconnected
  if ((zoo_connection_ != NULL) && ( zoo_connection_->GetConnectionTime() > 5 )  && !zoo_connection_->IsConnected()) {
    ResetToUnconnected();
  }

  switch (state_) {
    case unconnected:
    {
      // Try to connect with ZK
      LM_T(LmtClusterSetup, ("Trying to connect with zk at %s", zoo_host_.c_str()));
      zoo_connection_ = new zoo::Connection(zoo_host_, "samson", "samson");
      int rc = zoo_connection_->WaitUntilConnected(20000);
      if (rc) {
        state_message_ = au::str("Not possible to connect with zk at %s (%s)"
                                 , zoo_host_.c_str()
                                 , zoo::str_error(rc).c_str());
        zoo_connection_ = NULL;
        return;
      }

      // Main worker controller ( based on zookeeper connection )
      worker_controller_ = new SamsonWorkerController(zoo_connection_.shared_object(), port_, web_port_);
      rc = worker_controller_->init();
      if (rc) {
        state_message_ =  au::str("Error creating worker controller %s", zoo::str_error(rc).c_str());
        zoo_connection_ = NULL;
        worker_controller_ = NULL;
        return;
      }

      data_model_ = new DataModel(zoo_connection_.shared_object());
      network_ = new WorkerNetwork(worker_controller_->worker_id(), port_);


      state_ = connected;          // Now we are connected
      state_message_ = "Connected";
    }
    break;

    case connected:
    {
      // Let see if we promote to "included"...
      au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
      if (isWorkerIncluded(cluster_info.shared_object(), worker_controller_->worker_id())) {
        network_->set_cluster_information(cluster_info);   // Inform network about cluster setup
        state_message_ = "Connected & part of the cluster";
        state_ = included;
      } else {
        state_message_ = "Still not included in the cluster";
      }
    }
    break;

    case included:
    {
      // Review if we have all blocks to promoto to "ready"
      KVRanges hg_ranges = worker_controller_->GetAllMyKVRanges();   // Get a list of all the blocks I should have
      std::set<size_t> block_ids = data_model_->get_my_block_ids(hg_ranges);

      // Check if all the blocks are contained ( otherwise ask for them )
      std::set<size_t> own_block_ids = stream::BlockManager::shared()->GetBlockIds();

      // Get the list of pending block ids
      std::set<size_t> pending_block_ids;
      std::set<size_t>::const_iterator it;
      for (it = block_ids.begin(); it != block_ids.end(); it++) {
        if (own_block_ids.find(*it) == own_block_ids.end()) {
          pending_block_ids.insert(*it);
        }
      }

      // Add pending blocks if required
      if (pending_block_ids.size() > 0) {
        LM_W(("Missing %lu block.", pending_block_ids.size()));
        worker_block_manager_->RequestBlocks(pending_block_ids);
      }


      if (pending_block_ids.size() == 0) {
        // Reload all modules just before become ready
        ReloadModulesIfNecessary();

        state_ = ready;
        state_message_ = "ready";
        state_ = ready;
      } else {
        state_message_ = au::S() << "Pending " << pending_block_ids.size() << " blocks to be collected";
      }
    }
    break;

    case ready:
    {
      // Review all internal components
      worker_block_manager_->Review();
      task_manager_->review_stream_operations();
      worker_controller_->UpdateWorkerNode(true);
      ReloadModulesIfNecessary();

      // Remove all blocks not belonging to the cluster
      std::set<size_t> block_ids = data_model_->get_block_ids();
      std::set<size_t> worker_ids = worker_controller()->GetWorkerIds();
      stream::BlockManager::shared()->RemoveBlocksIfNecessary(block_ids, block_ids, worker_ids);

      // Promote to cluster ready?
      if (worker_controller_->IsClusterReady()) {
        state_ = cluster_ready;
        state_message_ = "Cluster ready";
      }
    }
    break;

    case cluster_ready:
    {
      // Review all internal components
      worker_block_manager_->Review();
      task_manager_->review_stream_operations();
      worker_controller_->UpdateWorkerNode(true);
      ReloadModulesIfNecessary();

      // Remove all blocks I should not have
      std::set<size_t> all_block_ids = data_model_->get_block_ids();
      KVRanges hg_ranges = worker_controller_->GetAllMyKVRanges();   // Get a list of all the blocks I should have
      std::set<size_t> my_block_ids = data_model_->get_my_block_ids(hg_ranges);
      std::set<size_t> worker_ids = worker_controller()->GetWorkerIds();
      stream::BlockManager::shared()->RemoveBlocksIfNecessary(all_block_ids, my_block_ids, worker_ids);
    }
    break;
  }
}

void SamsonWorker::ResetToUnconnected() {
  // New state
  state_ = unconnected;
  state_message_ = "Unconnected";


  zoo_connection_ = NULL;
  worker_controller_ = NULL;
  data_model_ = NULL;
  network_ = NULL;

  // Reset internal components
  worker_block_manager_->Reset();
  task_manager_->Reset();       // Reset current tasks
  last_modules_version_ = 0;    // Reset version of the modules
}

void SamsonWorker::ResetToConnected() {
  state_ = connected;
  state_message_ = "Reset to connected";

  task_manager_->Reset();
}

/* ****************************************************************************
 *
 * SamsonWorker::receive -
 */

void SamsonWorker::receive(const PacketPointer& packet) {
  LM_T(LmtNetworkNodeMessages, ("SamsonWorker received %s ", packet->str().c_str()));


  if (!IsConnected()) {
    LM_W(("Ignoring packet %s since we are not connected any more", packet->str().c_str()));
    return;
  }

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

    // Schedule operations to send this block to this worker
    if (stream::BlockManager::shared()->GetBlock(block_id) == NULL) {
      LM_W(("Received a Message::BlockRequest for unknown block %lu", block_id));

      // Return with an error
      PacketPointer p(new Packet(Message::BlockDistribution));
      p->to = packet->from;
      p->message->set_block_id(block_id);
      p->message->mutable_error()->set_message("Unknown block");
      network_->Send(p);
      return;
    }

    // Add the task for this request
    std::vector<size_t> worker_ids;
    worker_ids.push_back(worker_id);
    task_manager_->AddBlockDistributionTask(block_id, worker_ids);
    return;
  }


  // --------------------------------------------------------------------
  // PopBlockRequest
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
    if (stream::BlockManager::shared()->GetBlock(block_id) == NULL) {
      LM_W(("Unknown block_id(%d) in PopBlockRequest", block_id));
      p->message->mutable_error()->set_message("Unknown block");
    } else {
      // Schedule task
      au::SharedPointer<stream::WorkerTaskBase> task;
      task.Reset(new stream::PopBlockRequestTask( this
                                                 , task_manager_->getNewId()
                                                 , block_id
                                                 , ranges
                                                 , delilah_id
                                                 , delilah_component_id
                                                 , pop_id));
      task_manager_->Add(task);
    }

    // Send confirmation packet
    network_->Send(p);
    return;
  }

  // --------------------------------------------------------------------
  // Block distribution
  // --------------------------------------------------------------------

  if (msgCode == Message::BlockDistribution) {
    if (!packet->message->has_block_id()) {
      LM_W(("Received a Message::DuplicateBlock without block id. Ignoring..."));
      return;
    }

    if (packet->buffer() == NULL) {
      LM_W(("Received a Message::DuplicateBlock without a buffer. Ignoring..."));
      return;
    }

    if (packet->from.node_type != WorkerNode) {
      LM_W(("Received a Message::DuplicateBlock from a delilah client. Ignoring..."));
      return;
    }

    size_t block_id = packet->message->block_id();
    size_t worker_id = packet->from.id;

    if (block_id == (size_t)-1) {
      LM_W(("Received a Message::DuplicateBlock with incorrect block id"));
      return;
    }

    // Create the block in our block manager
    worker_block_manager_->CreateBlock(packet->buffer(), block_id);

    // Send message back
    PacketPointer p(new Packet(Message::BlockDistributionResponse));
    p->message->set_block_id(block_id);
    p->to = packet->from;
    network_->Send(p);

    // Notify that we have received this block
    worker_block_manager_->ReceivedBlockDistribution(block_id, worker_id, packet->buffer());
    return;
  }

  if (msgCode == Message::BlockDistributionResponse) {
    if (packet->from.node_type != WorkerNode) {
      LM_W(("Received a Message::BlockDistributionResponse from a delilah client. Ignoring..."));
      return;
    }

    if (!packet->message->has_block_id()) {
      LM_W(("Received a Message::BlockDistributionResponse with block id"));
      return;
    }

    size_t block_id = packet->message->block_id();
    size_t worker_id = packet->from.id;
    worker_block_manager_->ReceivedBlockDistributionResponse(block_id, worker_id);
    return;
  }

  // --------------------------------------------------------------------
  // push messages
  // --------------------------------------------------------------------

  if (msgCode == Message::PushBlockCommit) {
    if (!packet->message->has_push_id()) {
      LM_W(("Received a push block message without the push_id"));
      return;         // No sense to answer with this error
    } else if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;         // No sense to answer with this error
    }

    // Use push manager to deal with this type of message
    size_t push_id = packet->message->push_id();
    worker_block_manager_->receive_push_block_commit(packet->from.id, push_id);
    return;
  }


  if (msgCode == Message::PushBlock) {
    if (!packet->message->has_push_id()) {
      LM_W(("Received a push block message without the push_id"));
      return;         // No sense to answer with this error
    }

    if (packet->from.node_type != DelilahNode) {
      LM_W(("Received a push packet from a non delilah connection (%s)"
            , packet->from.str().c_str()));
      return;         // No sense to answer with this error
    }

    if (packet->from.id == 0) {
      LM_W(("Received a push packet from a delilah_id = 0. Rejected" ));
      return;         // No sense to answer with this error
    }

    if (packet->buffer() == NULL) {
      LM_W(("Received a push block message without a buffer with data"));
      return;         // No sense to answer with this error
    }

    // Use push manager to deal with this type of message
    size_t push_id = packet->message->push_id();
    std::vector<std::string> queues;
    for (int i = 0; i < packet->message->queue_size(); i++) {
      queues.push_back(packet->message->queue(i));
    }

    worker_block_manager_->receive_push_block(packet->from.id, push_id, packet->buffer(), queues);
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
    gpb_queue->set_key_format("?");         // It is necessary to fill this fields
    gpb_queue->set_value_format("?");
    // TODO: @andreu check the value that version should be initialized to
    gpb_queue->set_version(1);


    // Get a copy of the entire data model
    au::SharedPointer<gpb::Data> data = data_model_->getCurrentModel();

    if (commit_id == static_cast<size_t>(-1)) {
      // Duplicate queue and link
      au::ErrorManager error;
      data_model_->Commit("pop", au::str("add_queue_connection %s %s", original_queue.c_str(),
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
        for (int i = 0; i < queue->blocks_size(); ++i) {
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

    WorkerCommand *workerCommand = new WorkerCommand( this
                                                     , worker_command_id
                                                     , delilah_id
                                                     , delilah_component_id
                                                     , packet->message->worker_command()
                                                     );

    workerCommandManager_->Add(workerCommand);
    return;
  }

  LM_W(("Received a message with type %s. Just ignoring...", messageCode(msgCode)));
}

std::string SamsonWorker::str_state() {
  switch (state_) {
    case unconnected:
      return au::str("Not connected to ZK at %s:%d", zoo_host_.c_str(), port_);

      break;
    case connected:
      return au::str("Connected ZK at %s:%d. Still not included in cluster", zoo_host_.c_str(), port_);

      break;
    case included:
      return au::str("Connected ZK at %s:%d. Included in cluster, but not ready for processing",
                     zoo_host_.c_str(), port_);

      break;
    case ready:
      return au::str("Connected ZK at %s:%d and ready", zoo_host_.c_str(), port_);

    case cluster_ready:
      return au::str("Cluster ready at ZK at %s:%d", zoo_host_.c_str(), port_);

      break;
  }
  return "Unknown";
}

// Receive notifications
void SamsonWorker::notify(engine::Notification *notification) {
  if (notification->isName(notification_packet_received)) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet>("packet");
    if (packet == NULL) {
      LM_W(("Received a notification to receive a packet without a packet"));
    }
    receive(packet);
    return;
  }

  if (notification->isName("samson_worker_review")) {
    Review();
    return;
  }

  if (notification->isName("notification_cluster_info_changed_in_worker")) {
    // If we are not included or ready, we cannot process this notification
    if (state_ == unconnected) {
      LM_W(("New cluster setup cannot be processed since we are disconnected"));
      return;
    }

    // Recover new cluster setup
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
    LM_W(("Notification NEW CLUSTER SETUP version %lu", cluster_info->version()));

    // If I am not part of this cluster, do not set connections
    if (!isWorkerIncluded(cluster_info.shared_object(), worker_controller_->worker_id())) {
      LM_W(("Still not included in cluster info version %lu. Not seting up connections...", cluster_info->version()));
      state_ = connected;   // Connected but still not included in the cluster
      state_message_ = "Still not included in the cluster";
      return;
    }

    // Num of generated packets per operation equal to the number of workers
    ProcessIsolated::num_hg_divisions = cluster_info->workers_size();

    // Change network setup to adapt to the new scenario
    network_->set_cluster_information(cluster_info);

    // Reset everything necessary to come back to connected state
    ResetToConnected();

    // Show a label with all the new ranges I am responsible for
    KVRanges ranges = worker_controller_->GetMyKVRanges();
    LM_W(("Cluster setup change: Assgined ranges %s", ranges.str().c_str()));
    return;
  }

  if (notification->isName(notification_update_status)) {
    AU_LM_M(( str_state().c_str()));

    // Some ancient samson-0.6 useful Status information
    // Collect some information and print status...
    int num_processes = engine::Engine::process_manager()->num_used_procesors();
    int max_processes = engine::Engine::process_manager()->max_num_procesors();

    size_t used_memory = engine::Engine::memory_manager()->used_memory();
    size_t max_memory = engine::Engine::memory_manager()->memory();

    size_t disk_read_rate = (size_t) engine::Engine::disk_manager()->rate_in();
    size_t disk_write_rate = (size_t) engine::Engine::disk_manager()->rate_out();

    size_t network_read_rate = 0;
    size_t network_write_rate = 0;
    if (network_ != NULL) {
      network_read_rate = network_->get_rate_in();
      network_write_rate = network_->get_rate_out();
    }

    if (state_ ==  unconnected) {
      LM_W(("Worker unconnected"));
      return;
    }

    // Pointer to cluster info
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
    size_t worker_id = worker_controller_->worker_id();
    AU_LM_M(("Worker %lu %lu/%lu kv-ranges [ Cluster %lu workers ]"
          , worker_id
          , GetNumKVRanges(cluster_info.shared_object(), worker_id)
          , cluster_info->process_units_size()
          , cluster_info->workers_size()
          ));
    AU_LM_M(("Status (%s) [ P %s M %s D_in %s D_out %s N_in %s N_out %s ]"
          , au::str_time(cronometer_.seconds()).c_str()
          , au::str_percentage(num_processes, max_processes).c_str()
          , au::str_percentage(used_memory, max_memory).c_str()
          , au::str(disk_read_rate, "Bs").c_str()
          , au::str(disk_write_rate, "Bs").c_str()
          , au::str(network_read_rate, "Bs").c_str()
          , au::str(network_write_rate, "Bs").c_str()
          ));

    return;
  }

  if (notification->isName(notification_samson_worker_send_message)) {
    std::string message = notification->environment().Get("message", "No message coming with trace-notification");
    std::string context = notification->environment().Get("context", "?");
    std::string type    = notification->environment().Get("type", "message");

    size_t delilah_id = notification->environment().Get("delilah_id", (size_t) -1 );
    if( delilah_id == (size_t)-1)
      network_->SendAlertToAllDelilahs(type, context, message);
    else
      network_->SendAlertToDelilah(delilah_id , type, context, message);
    return;
  }

  LM_W(("SamsonWorker received an unexpected notification %s. Ignoring...", notification->GetDescription().c_str()));
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
  }
  return output.str();
}

std::string getFormatedError(std::string message, std::string& format) {
  return getFormatedElement("error", message, format);
}

void SamsonWorker::autoComplete(au::ConsoleAutoComplete *info) {
  if (info->completingFirstWord()) {
    info->add("quit");
    info->add("exit");
    info->add("threads");
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

  au::ErrorManager error;
  if (main_command == "log") {
    
    if( cmdLine.get_num_arguments() < 2 )
    {
      au::log_central.evalCommand("help" , error );
    }
    else
      au::log_central.evalCommand( cmdLine.get_argument(1) , error );

    // Write the output of the command
    write( &error );
    return;
  }
  
  
  if (main_command == "quit") {
    quitConsole();
  }
  if (main_command == "exit") {
    quitConsole();
  }
  if (main_command == "threads") {
    writeOnConsole(au::Singleton<au::ThreadManager>::shared()->str());
  }

  if (main_command == "show_engine_current_element") {
    writeOnConsole(engine::Engine::shared()->activity_monitor()->GetCurrentActivity() + "\n");
    return;
  }

  if (main_command == "show_engine_statistics") {
    writeOnConsole(engine::Engine::shared()->activity_monitor()->GetElementsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_last_items") {
    writeOnConsole(engine::Engine::shared()->activity_monitor()->GetLastItemsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_elements") {
    writeOnConsole(engine::Engine::shared()->GetTableOfEngineElements() + "\n");
    return;
  }


  // More command to check what is going on inside a worker
}

std::string SamsonWorker::getPrompt() {
  return "SamsonWorker> ";
}
  au::SharedPointer<gpb::Collection> SamsonWorker::GetWorkerCollection(const Visualization& visualization){

    au::SharedPointer<gpb::Collection> collection ( new gpb::Collection() );

  collection->set_name("workers");

  gpb::CollectionRecord *record = collection->add_record();

  if (visualization.get_flag("engine")) {
    size_t num_elements = engine::Engine::shared()->GetNumElementsInEngineStack();
    double waiting_time = engine::Engine::shared()->GetMaxWaitingTimeInEngineStack();
    size_t num_buffers = engine::Engine::memory_manager()->num_buffers();
    ::samson::add(record, "#buffers in memory", num_buffers, "f=uint64,sum");
    ::samson::add(record, "#elements in engine", num_elements, "f=uint64,sum");
    ::samson::add(record, "Max waiting time", waiting_time, "f=double,different");
  } else if (visualization.get_flag("disk")) {
    // Disk activiry

    ::samson::add(record, "Disk in B/s", engine::Engine::disk_manager()->rate_in(), "f=uint64,sum");
    ::samson::add(record, "Disk out B/s", engine::Engine::disk_manager()->rate_out(), "f=uint64,sum");
    double op_in = engine::Engine::disk_manager()->rate_operations_in();
    double op_out = engine::Engine::disk_manager()->rate_operations_out();
    ::samson::add(record, "Disk in Ops/s", op_in, "f=double , sum");
    ::samson::add(record, "Disk out Ops/s", op_out, "f=double,sum");
    double on_time = engine::Engine::disk_manager()->on_time();
    double off_time = engine::Engine::disk_manager()->off_time();
    ::samson::add(record, "On time", on_time, "f=double,differet");
    ::samson::add(record, "Off time", off_time, "f=double,differet");
    ::samson::add(record, "BM writing",  stream::BlockManager::shared()->scheduled_write_size(), "f=uint64,sum");
    ::samson::add(record, "BM reading",  stream::BlockManager::shared()->scheduled_read_size(), "f=uint64,sum");
    double usage =  engine::Engine::disk_manager()->on_off_activity();
    ::samson::add(record, "Disk usage", au::str_percentage(usage), "differet");
  } else {
    // Add ready flag
    ::samson::add(record, "Status", state_message_, "different");
    ::samson::add(record, "Mem used", engine::Engine::memory_manager()->used_memory(), "f=uint64,sum");
    ::samson::add(record, "Mem total", engine::Engine::memory_manager()->memory(), "f=uint64,sum");
    ::samson::add(record, "Cores used", engine::Engine::process_manager()->num_used_procesors(), "f=uint64,sum");
    ::samson::add(record, "Cores total", engine::Engine::process_manager()->max_num_procesors(), "f=uint64,sum");
    ::samson::add(record, "#Disk ops", engine::Engine::disk_manager()->num_disk_operations(), "f=uint64,sum");
    ::samson::add(record, "Disk in B/s", engine::Engine::disk_manager()->rate_in(), "f=uint64,sum");
    ::samson::add(record, "Disk out B/s", engine::Engine::disk_manager()->rate_out(), "f=uint64,sum");
    ::samson::add(record, "Net in B/s", network_->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "Net out B/s", network_->get_rate_out(), "f=uint64,sum");
  }

  return collection;
}

au::SharedPointer<zoo::Connection> SamsonWorker::zoo_connection() {
  return zoo_connection_;
};
au::SharedPointer<SamsonWorkerController> SamsonWorker::worker_controller() {
  return worker_controller_;
};
au::SharedPointer<DataModel> SamsonWorker::data_model() {
  return data_model_;
};
au::SharedPointer<WorkerNetwork> SamsonWorker::network() {
  return network_;
};

au::SharedPointer<SamsonWorkerRest> SamsonWorker::samson_worker_rest() {
  return samson_worker_rest_;
}

au::SharedPointer<WorkerBlockManager> SamsonWorker::worker_block_manager() {
  return worker_block_manager_;
}

au::SharedPointer<stream::WorkerTaskManager> SamsonWorker::task_manager() {
  return task_manager_;
};
au::SharedPointer<WorkerCommandManager> SamsonWorker::workerCommandManager() {
  return workerCommandManager_;
}

void SamsonWorker::ReloadModulesIfNecessary() {
  // Recover current data model
  au::SharedPointer<gpb::Data> data = data_model_->getCurrentModel();

  // Get .modules queue
  gpb::Queue *queue = get_queue(data.shared_object(), ".modules");

  if (!queue) {
    if (last_modules_version_ != 0) {
      std::string directory = au::Singleton<SamsonSetup>::shared()->worker_modules_directory();

      // Clear modules and remove directory
      au::Singleton<ModulesManager>::shared()->clearModulesManager();
      au::ErrorManager error;
      au::removeDirectory(directory, error);

      last_modules_version_ = 0;
    }
    return;
  }

  size_t version = queue->version();
  if (version <= last_modules_version_) {
    return;   // Not necessary to update
  }
  last_modules_version_ = version;

  std::string directory = au::Singleton<SamsonSetup>::shared()->worker_modules_directory();

  // Clear modules
  au::Singleton<ModulesManager>::shared()->clearModulesManager();

  // Clear directory
  au::ErrorManager error;
  au::removeDirectory(directory, error);
  createDirectory(directory);

  // Write each loaded file
  for (int i = 0; i < queue->blocks_size(); i++) {
    size_t block_id = queue->blocks(i).block_id();

    // Recover block
    stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id);

    if (block == NULL) {
      LM_W(("Block %lu necessary for a module not found. Skipping..."));
      continue;
    }

    engine::BufferPointer buffer = block->buffer();

    if (buffer == NULL) {
      // Write file
      std::string source_file_name = block->file_name();
      size_t file_size = au::sizeOfFile(source_file_name);

      buffer = engine::Buffer::Create("module", "module", file_size);
      au::ErrorManager error_writing_file;
      buffer->WriteFile(source_file_name, error_writing_file);

      if (error_writing_file.IsActivated()) {
        LM_W(("Error reading module file %s", source_file_name.c_str()));
        continue;
      }
    }

    std::string target_file_name = au::str("%s/module_%d", directory.c_str(), i);
    FILE *target_file = fopen(target_file_name.c_str(), "w");
    fwrite(buffer->data() + sizeof(KVHeader), buffer->size() - sizeof(KVHeader), 1, target_file);

    fclose(target_file);
  }

  // Reload modules from this directory
  au::Singleton<ModulesManager>::shared()->addModulesFromDirectory(directory);
}

bool SamsonWorker::IsReady() {
  if (state_ == ready) {
    return true;
  }
  if (state_ == cluster_ready) {
    return true;
  }
  return false;
}

bool SamsonWorker::IsConnected() {
  return state_ != unconnected;
};
}

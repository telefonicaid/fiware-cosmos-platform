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


#include <dirent.h>
#include <dlfcn.h>
#include <iconv.h>
#include <iostream>                               // std::cout ...
#include <sys/types.h>

#include "json.h"

#include "logMsg/logMsg.h"                        // lmInit, LM_*
#include "logMsg/traceLevels.h"                   // Trace Levels

#include "au/CommandLine.h"                       // CommandLine
#include "au/ThreadManager.h"
#include "au/file.h"
#include "au/log/LogMain.h"
#include "au/network/RESTServiceCommand.h"
#include "au/string/S.h"
#include "au/string/StringUtilities.h"            // au::Format
#include "au/tables/pugixml.hpp"                  // pugixml
#include "au/time.h"                              // au::todayString

#include "engine/DiskManager.h"                   // Notifications
#include "engine/DiskOperation.h"                 // samson::DiskOperation
#include "engine/Engine.h"                        // engine::Engine
#include "engine/MemoryManager.h"                 // samson::SharedMemory
#include "engine/Notification.h"                  // engine::Notification
#include "engine/ProcessManager.h"                // engine::ProcessManager

#include "samson/common/KVRange.h"
#include "samson/common/Logs.h"
#include "samson/common/Macros.h"                 // EXIT, ...
#include "samson/common/MessagesOperations.h"
#include "samson/common/NotificationMessages.h"
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/SamsonSetup.h"            // samson::SamsonSetup
#include "samson/common/gpb_operations.h"
#include "samson/delilah/WorkerCommandDelilahComponent.h"
#include "samson/module/ModulesManager.h"         // samson::ModulesManager
#include "samson/module/samsonVersion.h"          // SAMSON_VERSION
#include "samson/network/Message.h"               // Message
#include "samson/network/NetworkInterface.h"      // NetworkInterface
#include "samson/network/NetworkInterface.h"      // samson::NetworkInterface
#include "samson/network/Packet.h"                // samson::Packet
#include "samson/network/WorkerNetwork.h"
#include "samson/stream/Block.h"                  // samson::stream::Block
#include "samson/stream/BlockList.h"              // samson::stream::BlockList
#include "samson/stream/BlockManager.h"           // samson::stream::BlockManager
#include "samson/stream/SharedMemoryManager.h"  // samson::SharedMemoryManager
#include "samson/stream/WorkerTaskManager.h"
#include "samson/worker/SamsonWorkerRest.h"
#include "samson/worker/WorkerCommand.h"  // samson::stream::WorkerCommand

#include "samson/worker/SamsonWorker.h"  // Own interfce

#define SAMSON_ERROR_OVERLOADED    "Overloaded worker"
#define SAMSON_ERROR_UNKNOWN_BLOCK "Unknown block"

namespace samson {
/* ****************************************************************************
 *
 * Constructor
 */

SamsonWorker::SamsonWorker(std::string zoo_host, int port, int web_port) :
  zoo_host_(zoo_host)
  , port_(port)
  , web_port_(web_port)
  , zk_first_connection_(true)
  , last_modules_version_(SIZE_T_UNDEFINED) {
  // Random initialization
  srand(time(NULL));

  // Components
  worker_block_manager_.Reset(new WorkerBlockManager(this));
  workerCommandManager_ = new WorkerCommandManager(this);
  samson_worker_rest_ = new SamsonWorkerRest(this, web_port_);
  task_manager_ =  new stream::WorkerTaskManager(this);
  network_ = NULL;  // network_ will be properly initialized later


  // Initial state of this worker ( unconnected )
  state_ = unconnected;
  state_message_ = "Worker starting...";

  listen(notification_samson_worker_send_message);              // Listen this notification to send traces
  listen(notification_update_status);                           // Notification to update state
  listen("notification_cluster_info_changed_in_worker");        // Cluster setup changes
  listen("samson_worker_review");                               // Review notification ( in this review all connections are stablished )
  listen(notification_packet_received);
  listen("notification_freeze_data_model");                     // Notification to freexe data model periodically
  listen("notification_new_cluster_setup");

  samson::SamsonSetup *samson_setup = au::Singleton<samson::SamsonSetup>::shared();

  engine::notify(notification_update_status, samson_setup->GetInt("general.update_status_period"));
  engine::notify(notification_samson_worker_check_finish_tasks, samson_setup->GetInt("worker.period_check_finish_tasks"));
  engine::notify("samson_worker_review", 2);
  engine::notify("notification_freeze_data_model", samson_setup->GetInt("worker.period_to_freeze_data_model"));

  engine::notify_extra("samson_worker_review");
}

void SamsonWorker::Review() {
  // If zoo connection is not valid, come back to unconnected
  if ((zoo_connection_ != NULL) && (zoo_connection_->GetConnectionTime() > 5)  && !zoo_connection_->IsConnected()) {
    ResetToUnconnected();
  }

  switch (state_) {
    case unconnected:
    {
      if (!zk_first_connection_  && zk_connection_cronometer_.seconds() < 2) {
        return;
      }
      zk_connection_cronometer_.Reset();
      zk_first_connection_ = false;

      // Try to connect with ZK
      LOG_V(logs.worker_controller, ("Trying to connect to zk at %s", zoo_host_.c_str()));
      zoo_connection_ = new au::zoo::Connection(zoo_host_, "samson", "samson");
      int rc = zoo_connection_->WaitUntilConnected(20000);
      if (rc) {
        state_message_ = au::str("Unable to connect to zk at %s (%s)"
                                 , zoo_host_.c_str()
                                 , au::zoo::str_error(rc).c_str());
        LOG_SW(("%s", state_message_.c_str()));
        zoo_connection_ = NULL;
        return;
      }

      // Main worker controller ( based on zookeeper connection )
      worker_controller_ = new SamsonWorkerController(zoo_connection_.shared_object(), port_, web_port_);
      rc = worker_controller_->init();
      if (rc) {
        state_message_ =  au::str("Error creating worker controller %s", au::zoo::str_error(rc).c_str());
        LOG_SW(("%s", state_message_.c_str()));
        zoo_connection_ = NULL;
        worker_controller_ = NULL;
        return;
      }

      data_model_ = new DataModel(zoo_connection_);
      data_model_->UpdateToLastVersion();
      network_ = new WorkerNetwork(worker_controller_->worker_id(), port_);

      state_ = connected;            // Now we are connected
      state_message_ = "Connected";
      LOG_V(logs.worker_controller, ("Worker connected to Zookeeper"));
      break;
    }
    case connected:
    {
      // Let see if we promote to "included"...
      au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
      if (isWorkerIncluded(cluster_info.shared_object(), worker_controller_->worker_id())) {
        network_->set_cluster_information(cluster_info);     // Inform network about cluster setup
        state_message_ = "Ready";
        state_ = ready;
        LOG_M(logs.worker, ("Worker connected to Zookeeper. This worker is now included in the cluster"));
      } else {
        state_message_ = "Still not included in the cluster";
      }
    }
    break;

    case ready:
    {
      // Review all internal components
      worker_block_manager_->Review();
      task_manager_->review_stream_operations();
      ReloadModulesIfNecessary();

      // Take a list of all the blocks considered in data model
      std::set<size_t> all_block_ids = data_model_->GetAllBlockIds();

      // Remove all block request for blocks not belonging to data model
      worker_block_manager_->RemoveRequestIfNecessary(all_block_ids);

      // Request blocks I am suppouse to have for previous and possible candidate data-models
      std::vector<KVRange> my_ranges = worker_controller_->GetAllMyKVRanges();
      std::set<size_t> my_block_ids = data_model_->GetMyBlockIdsForPreviousAndCandidateDataModel(my_ranges);
      for (std::set<size_t>::iterator it = my_block_ids.begin(); it != my_block_ids.end(); ++it) {
        if (stream::BlockManager::shared()->GetBlock(*it) != NULL) {
          continue;  // This block is contained in the local manager
        }
        worker_block_manager_->RequestBlock(*it);        // Add block request
      }

      // Check what commit_id can guarantee this worker
      size_t last_commit_id = worker_controller_->GetMyLastCommitId();  // This is the currently guanrantee commit_id
      size_t current_data_commit_id = data_model_->GetLastCommitIdForCurrentDataModel();
      size_t previous_data_commit_id = data_model_->GetLastCommitIdForPreviousDataModel();
      size_t candidate_data_commit_id = data_model_->GetLastCommitIdForCandidateDataModel();

      // Confirm all push block operations previous to this commit
      worker_block_manager_->ReviewPushItems(previous_data_commit_id, current_data_commit_id);

      if (previous_data_commit_id > last_commit_id) {
        // Check I can confirm this level of data model
        std::set<size_t> block_ids = data_model_->GetMyBlockIdsForPreviousDataModel(my_ranges);
        if (stream::BlockManager::shared()->CheckBlocks(block_ids)) {
          last_commit_id = previous_data_commit_id;
        }
      }

      if (candidate_data_commit_id != static_cast<size_t>(-1)) {
        if (candidate_data_commit_id > last_commit_id) {
          // Check I can confirm this level of data model
          std::set<size_t> block_ids = data_model_->GetMyBlockIdsForCandidateDataModel(my_ranges);
          if (stream::BlockManager::shared()->CheckBlocks(block_ids)) {
            last_commit_id = candidate_data_commit_id;
          }
        }
      }

      // Update if necessary my current information
      worker_controller_->UpdateWorkerNode(last_commit_id);

      // Check to see if I need to remove any pop queue or queue_connections for disconnected delilah clients
      if (worker_controller_->cluster_leader()) {
        ReviewPopQueues();
      }

      // If I am the cluster leader, consolidate next frozen data model if necessary
      if (worker_controller_->cluster_leader()) {
        size_t candidate_data_commit_id = data_model_->GetLastCommitIdForCandidateDataModel();

        if (candidate_data_commit_id != static_cast<size_t>(-1)) {
          // Checkk all workers satisfy this level of data model
          if (worker_controller_->CheckDataModelCommitId(candidate_data_commit_id)) {
            data_model_->FreezeCandidateDataModel();
          }
        }
      }
    }
    break;
  }
}

void SamsonWorker::ReviewPopQueues() {
  au::SharedPointer<gpb::DataModel> data_model =  data_model_->getCurrentModel();
  gpb::Data *current_data = data_model->mutable_current_data();

  for (int i = 0; i < current_data->queue_size(); i++) {
    std::string queue = current_data->queue(i).name();
    if (queue.substr(0, 5) == ".pop_") {           // Pop queues names matchs ".pop_delilah_id_XXX"
      size_t p = queue.find('_', 5);
      if (p == std::string::npos) {
        LOG_W(logs.worker, ("Strange queue format for pop queue %s", queue.c_str()));
        continue;
      }
      size_t delilah_id = au::code64_num(queue.substr(5, p - 5));
      // Check if this delilah is connected or has been connected in the last minute
      if (!network_->CheckValidNode(NodeIdentifier(DelilahNode, delilah_id))) {
        // Remove this queue
        au::ErrorManager error;
        std::string command = au::str("rm %s", queue.c_str());
        data_model_->Commit("pop_review", command, error);
      }
    }
  }

  for (int i = 0; i < current_data->queue_connections_size(); i++) {
    std::string source_queue = current_data->queue_connections(i).queue_source();
    std::string queue = current_data->queue_connections(i).queue_target();
    if (queue.substr(0, 5) == ".pop_") {           // Pop queues names matchs ".pop_delilah_id_XXX"
      size_t p = queue.find('_', 5);
      if (p == std::string::npos) {
        LOG_W(logs.worker, ("Strange queue format for pop queue %s", queue.c_str()));
        continue;
      }
      size_t delilah_id = au::code64_num(queue.substr(5, p - 5));
      // Check if this delilah is connected or has been connected in the last minute
      if (!network_->CheckValidNode(NodeIdentifier(DelilahNode, delilah_id))) {
        // Remove this queue
        au::ErrorManager error;
        std::string command = au::str("rm_queue_connection %s %s", source_queue.c_str(), queue.c_str());
        data_model_->Commit("pop_review", command, error);
      }
    }
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
  task_manager_->Reset();         // Reset current tasks
  last_modules_version_ = SIZE_T_UNDEFINED;      // Reset version of the modules
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
  LOG_V(logs.in_messages, ("Received packet from %s : %s", packet->from.str().c_str(), packet->str().c_str()));

  if (!IsConnected()) {
    LOG_SW(("Ignoring packet %s since we are not connected any more", packet->str().c_str()));
    return;
  }

  // Type of message received
  Message::MessageCode msgCode = packet->msgCode;


  // If an overload-error is received, add an alert to not send more request to this worker in the next seconds....
  if (packet->from.node_type == WorkerNode) {
    if (packet->message->has_error() && (packet->message->error().message() == SAMSON_ERROR_OVERLOADED)) {
      worker_alert_.AddAlert(packet->from.id, 5);  // 5 seconds of peace for this worker
    }
  }

  // --------------------------------------------------------------------
  // StatusReport
  // --------------------------------------------------------------------

  if (msgCode == Message::StatusReport) {
    // LOG_SM(("Recieved status report message from %s" , packet->from.str().c_str() ));
    return;
  }


  // --------------------------------------------------------------------
  // BlockRequest
  // --------------------------------------------------------------------

  if (msgCode == Message::BlockRequest) {
    if (!packet->message->has_block_id()) {
      LOG_W(logs.worker, ("Received a Message::BlockRequest without block id"));
      return;
    }

    if (packet->from.node_type == DelilahNode) {
      LOG_W(logs.worker, ("Received a Message::BlockRequest from a delilah node"));
      return;
    }

    // Collect information for this request
    size_t block_id  = packet->message->block_id();
    stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id);
    size_t worker_id = packet->from.id;

    // Check if we have this block
    if (block == NULL) {
      LOG_V(logs.block_request, ("Received block request for %s from worker %lu. Unknown block... returning error"
                                 , str_block_id(block_id).c_str()
                                 , packet->from.id));
      PacketPointer p(new Packet(Message::BlockRequestResponse));
      p->to = packet->from;
      p->message->set_block_id(block_id);
      p->message->mutable_error()->set_message(SAMSON_ERROR_UNKNOWN_BLOCK);
      network_->Send(p);
      return;
    }

    // Check if this request has to be rejected because I am overloaded
    if (!IsWorkerReadyForBlockRequest(worker_id)) {
      PacketPointer p(new Packet(Message::BlockRequestResponse));
      p->to = packet->from;
      p->message->set_block_id(block_id);
      p->message->mutable_error()->set_message(SAMSON_ERROR_OVERLOADED);
      network_->Send(p);
    }

    // If block is in memory, we can anser rigth now
    if (block->is_content_in_memory()) {
      engine::BufferPointer buffer = block->buffer();
      if (buffer == NULL) {
        LM_X(1, ("Internal error"));
      }

      LOG_V(logs.block_request, ("Received block request for %s from worker %lu. Answering now since block is in mem"
                                 , str_block_id(block_id).c_str()
                                 , packet->from.id));

      PacketPointer p(new Packet(Message::BlockRequestResponse));
      p->to = packet->from;
      p->message->set_block_id(block_id);
      p->set_buffer(buffer);

      LOG_V(logs.block_request, ("Sending packet %s to %s"
                                 , p->str().c_str()
                                 , p->to.str().c_str()));
      network_->Send(p);
      return;
    }

    // Schedule a task to make to answer this block request ( loading block in memory when possible )
    std::vector<size_t> worker_ids;
    worker_ids.push_back(packet->from.id);
    size_t task_id = task_manager_->AddBlockRequestTask(block_id, worker_ids);
    LOG_V(logs.block_request, ("Received block request for %s from worker %lu. Scheduling task W%lu"
                               , str_block_id(block_id).c_str()
                               , packet->from.id
                               , task_id));
    return;
  }

  // --------------------------------------------------------------------
  // BlockRequestResponse
  // --------------------------------------------------------------------

  if (msgCode == Message::BlockRequestResponse) {
    if (!packet->message->has_block_id()) {
      LOG_SW(("Received a Message::BlockRequestResponse without block id. Ignoring..."));
      return;
    }

    if (packet->from.node_type != WorkerNode) {
      LOG_SW(("Received a Message::BlockRequestResponse from a delilah client. Ignoring..."));
      return;
    }

    size_t block_id = packet->message->block_id();
    size_t worker_id = packet->from.id;

    if (block_id == static_cast<size_t>(-1)) {
      LOG_SW(("Received a Message::DuplicateBlock with incorrect block id"));
      return;
    }

    if (packet->message->has_error()) {
      std::string error_message = packet->message->error().message();
      LOG_V(logs.block_request, ("Received block request response for %s from worker %lu with error %s"
                                 , str_block_id(block_id).c_str()
                                 , worker_id
                                 , error_message.c_str()
                                 ));
      worker_block_manager_->ReceivedBlockRequestResponse(block_id, worker_id, error_message);
      return;
    }

    if (packet->buffer() == NULL) {
      LOG_W(logs.block_request, ("Received a Message::BlockRequestResponse without a buffer. Ignoring..."));
      return;
    }

    LOG_V(logs.block_request, ("Received block request response for %s from worker %lu"
                               , str_block_id(block_id).c_str()
                               , worker_id
                               ));

    // Add the block to the block manager
    if (stream::BlockManager::shared()->GetBlock(block_id) == NULL) {
      stream::BlockManager::shared()->CreateBlock(block_id, packet->buffer());
    }

    // Notify about the correct reception of this block
    worker_block_manager_->ReceivedBlockRequestResponse(block_id, worker_id);
    return;
  }

  // --------------------------------------------------------------------
  // PopBlockRequest
  // --------------------------------------------------------------------

  if (msgCode == Message::PopBlockRequest) {
    if (!packet->message->has_block_id()) {
      LOG_SW(("Received an incorrect Message::PopBlockRequest. No block id"));
      return;
    }


    if (packet->from.node_type != DelilahNode) {
      LOG_SW(("Received a Message::PopBlockRequest from a worker node"));
      return;
    }

    size_t block_id  = packet->message->block_id();
    size_t delilah_id = packet->from.id;
    size_t delilah_component_id = packet->message->delilah_component_id();
    size_t pop_id = packet->message->pop_id();

    PacketPointer p(new Packet(Message::PopBlockRequestConfirmation, packet->from));
    p->message->set_delilah_component_id(packet->message->delilah_component_id());
    p->message->set_pop_id(packet->message->pop_id());

    // Schedule operations to send this block to this user
    if (stream::BlockManager::shared()->GetBlock(block_id) == NULL) {
      LOG_SW(("Unknown block_id(%d) in PopBlockRequest", block_id));
      p->message->mutable_error()->set_message("Unknown block");
    } else {
      // Schedule task
      au::SharedPointer<stream::WorkerTaskBase> task;
      task.Reset(new stream::PopBlockRequestTask(this
                                                 , task_manager_->getNewId()
                                                 , block_id
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
  // push messages
  // --------------------------------------------------------------------

  if (msgCode == Message::PushBlock) {
    if (!packet->message->has_push_id()) {
      LOG_SW(("Received a push block message without the push_id"));
      return;
    }

    if (packet->from.node_type != DelilahNode) {
      LOG_SW(("Received a push packet from a non delilah connection (%s)", packet->from.str().c_str()));
      return;
    }

    if (packet->from.id == 0) {
      LOG_SW(("Received a push packet from a delilah_id = 0. Rejected"));
      return;
    }

    if (packet->buffer() == NULL) {
      LOG_SW(("Received a push block message without a buffer with data"));
      return;
    }

    // Recover push_id and the list queue queues
    size_t push_id = packet->message->push_id();
    std::vector<std::string> queues;
    for (int i = 0; i < packet->message->queue_size(); ++i) {
      queues.push_back(packet->message->queue(i));
    }

    worker_block_manager_->ReceivedPushBlock(packet->from.id, push_id, packet->buffer(), queues);
  }

  // --------------------------------------------------------------------
  // pop messages
  // --------------------------------------------------------------------

  if (msgCode == Message::PopQueue) {
    if (!packet->message->has_pop_queue()) {
      LOG_SW(("Received a pop message without pop information.Ignoring..."));
      return;
    }

    size_t commit_id = packet->message->pop_queue().commit_id();
    size_t min_commit_id = packet->message->pop_queue().min_commit_id();
    size_t delilah_id = packet->from.id;
    std::string original_queue = packet->message->pop_queue().queue();
    std::string pop_queue = au::str(".pop_%s_%lu_%s"
                                    , au::code64_str(delilah_id).c_str()
                                    , packet->message->delilah_component_id()
                                    , original_queue.c_str());

    // Generate packet with all queue content
    au::SharedPointer<Packet> p(new Packet(Message::PopQueueResponse));
    p->message->set_delilah_component_id(packet->message->delilah_component_id());
    p->to = packet->from;
    gpb::Queue *gpb_queue = p->message->mutable_pop_queue_response()->mutable_queue();
    gpb_queue->set_name(original_queue);
    gpb_queue->set_key_format("?");             // It is necessary to fill these fields
    gpb_queue->set_value_format("?");
    gpb_queue->set_commit_id(SIZE_T_UNDEFINED);


    // Get a copy of the entire data model
    au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();
    gpb::Data *data = data_model->mutable_current_data();

    if (commit_id == static_cast<size_t>(-1)) {
      // Duplicate queue and link
      au::ErrorManager error;
      std::string command = au::str("add_queue_connection %s %s", original_queue.c_str(), pop_queue.c_str());
      data_model_->Commit("pop", command, error);

      if (error.HasErrors()) {
        LOG_SW(("Internal error with add_queue_connection command in pop request: %s", error.GetLastError().c_str()));
        return;
      }

      // If the queue really exist, return all its content to be popped
      gpb::Queue *queue = get_queue(data, original_queue);
      if (queue) {
        gpb_queue->CopyFrom(*queue);
      }
    } else {
      // Copy blocks newer than commit_id
      gpb::Queue *queue = get_queue(data, pop_queue);
      if (queue) {
        for (int i = 0; i < queue->blocks_size(); ++i) {
          if (queue->blocks(i).commit_id() > commit_id) {
            gpb_queue->add_blocks()->CopyFrom(queue->blocks(i));
          }
        }
      }

      // Remove old unnecessary blocks from queue
      if (queue) {
        CommitCommand commit_command;
        for (int i = 0; i < queue->blocks_size(); ++i) {
          if (queue->blocks(i).commit_id() <= commit_id) {  // Not new
            if ((min_commit_id == SIZE_T_UNDEFINED) || (queue->blocks(i).commit_id() < min_commit_id)) {
              // Add to be removed from this queue
              size_t block_id =  queue->blocks(i).block_id();
              KVFormat format(queue->key_format(), queue->value_format());
              KVRange range = queue->blocks(i).range();  // Implicit conversion
              KVInfo info(queue->blocks(i).size(), queue->blocks(i).kvs());
              commit_command.RemoveBlock(pop_queue, block_id, queue->blocks(i).block_size(), format, range, info);
            }
          }
        }
        if (commit_command.size() > 0) {
          au::ErrorManager error;
          data_model_->Commit("pop", commit_command.GetCommitCommand(), error);
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
      LOG_SW(("Trying to run a WorkerCommand from a packet without that message"));
      return;
    }

    size_t delilah_id = packet->from.id;
    size_t delilah_component_id = packet->message->delilah_component_id();

    std::string worker_command_id = au::str("%s_%lu", au::code64_str(delilah_id).c_str(), delilah_component_id);

    WorkerCommand *workerCommand = new WorkerCommand(this
                                                     , worker_command_id
                                                     , delilah_id
                                                     , delilah_component_id
                                                     , packet->message->worker_command()
                                                     );

    workerCommandManager_->Add(workerCommand);
    return;
  }

  LOG_SW(("Received a message with type %s. Just ignoring...", messageCode(msgCode)));
}

std::string SamsonWorker::str_state() {
  switch (state_) {
    case unconnected:
      return au::str("Not connected to ZK at %s:%d", zoo_host_.c_str(), port_);

      break;
    case connected:
      return au::str("Connected ZK at %s:%d. Still not included in cluster", zoo_host_.c_str(), port_);

      break;
    case ready:
      return au::str("Connected ZK at %s:%d & included in cluster", zoo_host_.c_str(), port_);

      break;
  }
  return "Unknown";
}

// Receive notifications
void SamsonWorker::notify(engine::Notification *notification) {
  if (notification->isName("notification_new_cluster_setup")) {
    au::ErrorManager error;
    data_model_->Commit("SAMSON cluster leader", "data_model_recover", error);
    LOG_W(logs.worker, ("New cluster setup, so data model is recovered"));
    return;
  }

  if (notification->isName("notification_freeze_data_model")) {
    if (worker_controller_ == NULL) {
      return;
    }

    if (!worker_controller_->cluster_leader()) {
      return;
    }

    // Freeze data model ( Create a candidate and so.... )
    au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();
    if (data_model == NULL) {
      return;
    }

    // If a candidate data model is too old, remove it
    if (data_model->has_candidate_data()) {
      if (cronometer_candidate_data_model_.seconds() > 300) {
        LOG_W(logs.worker, ("Canceling data model candidate for timeout"));
        au::ErrorManager error;
        data_model_->Commit("SAMSON cluster leader", "data_model_cancel_freeze", error);
      }
    }

    // Create a candidate if necessary
    if (!data_model->has_candidate_data()) {
      if (data_model->current_data().commit_id() > data_model->previous_data().commit_id()) {
        au::ErrorManager error;
        data_model_->Commit("SAMSON cluster leader", "data_model_freeze", error);
        cronometer_candidate_data_model_.Reset();
      }
    }
    return;
  }

  if (notification->isName(notification_packet_received)) {
    au::SharedPointer<Packet> packet = notification->dictionary().Get<Packet>("packet");
    if (packet == NULL) {
      LOG_SW(("Received a notification to receive a packet without a packet"));
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
      LOG_SW(("New cluster setup cannot be processed since we are disconnected"));
      return;
    }

    // Recover new cluster setup
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
    LOG_M(logs.worker, ("New cluster setup (version %lu)", cluster_info->version()));

    // If I am not part of this cluster, do not set connections
    if (!isWorkerIncluded(cluster_info.shared_object(), worker_controller_->worker_id())) {
      LOG_M(logs.worker,
            ("Still not included in cluster (version %lu). Not seting up this worker", cluster_info->version()));
      state_ = connected;     // Connected but still not included in the cluster
      state_message_ = "Still not included in the cluster";
      return;
    }


    // Change network setup to adapt to the new scenario
    network_->set_cluster_information(cluster_info);

    // Reset everything necessary to come back to connected state
    ResetToConnected();

    // Show a label with all the new ranges I am responsible for
    std::vector<KVRange> ranges = worker_controller_->GetMyKVRanges();
    LOG_M(logs.worker, ("Cluster setup change: Assgined ranges %s", str(ranges).c_str()));
    return;
  }

  if (notification->isName(notification_update_status)) {
    if ((state_ ==  connected) || (state_ ==  unconnected)) {
      LOG_W(logs.worker, ("Not reporting state of this worker since it is not fully connected to ZK"));
      return;
    }

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

    // Pointer to cluster info
    au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();
    size_t worker_id = worker_controller_->worker_id();
    LOG_M(logs.worker,
          ("[ Worker %lu / %d workers ] (%s) [ %d/%d HashGroups ][ P %s M %s D_in %s D_out %s N_in %s N_out %s ]"
           , worker_id
           , cluster_info->workers_size()
           , au::str_time(cronometer_.seconds()).c_str()
           , static_cast<int>(GetNumKVRanges(cluster_info.shared_object(), worker_id))
           , cluster_info->process_units_size()
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

    size_t delilah_id = notification->environment().Get("delilah_id", static_cast<size_t>(-1));
    if (delilah_id == static_cast<size_t>(-1)) {
      network_->SendAlertToAllDelilahs(type, context, message);
    } else {
      network_->SendAlertToDelilah(delilah_id, type, context, message);
    }
    return;
  }

  LOG_SW(("SamsonWorker received an unexpected notification %s. Ignoring...", notification->GetDescription().c_str()));
}

std::string getFormatedElement(const std::string& name, const std::string& value, const std::string& format) {
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

void SamsonWorker::AutoComplete(au::console::ConsoleAutoComplete *info) {
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

void SamsonWorker::EvalCommand(const std::string& command) {
  au::CommandLine cmdLine;

  cmdLine.Parse(command);
  if (cmdLine.get_num_arguments() == 0) {
    return;
  }

  std::string main_command = cmdLine.get_argument(0);

  au::ErrorManager error;

  if (au::CheckIfStringsBeginWith(main_command, "log_")) {
    au::log_central->EvalCommand(command, error);
    Write(error);          // Write the output of the command
    return;
  }

  if (main_command == "quit") {
    StopConsole();
    return;
  }
  if (main_command == "exit") {
    StopConsole();
    return;
  }

  if (main_command == "threads") {
    Write(au::Singleton<au::ThreadManager>::shared()->str_table());
    return;
  }

  if (main_command == "show_engine_current_element") {
    Write(engine::Engine::shared()->activity_monitor()->GetCurrentActivity() + "\n");
    return;
  }

  if (main_command == "show_engine_statistics") {
    Write(engine::Engine::shared()->activity_monitor()->GetElementsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_last_items") {
    Write(engine::Engine::shared()->activity_monitor()->GetLastItemsTable() + "\n");
    return;
  }

  if (main_command == "show_engine_elements") {
    Write(engine::Engine::shared()->GetTableOfEngineElements() + "\n");
    return;
  }

  // More command to check what is going on inside a worker
  WriteErrorOnConsole("Unknown command for samsonWorker");
}

std::string SamsonWorker::GetPrompt() {
  if (worker_controller_ == NULL) {
    return "[Unconnected] SamsonWorker > ";
  }

  size_t worker_id = worker_controller_->worker_id();
  if (worker_id == static_cast<size_t>(-1)) {
    return "[Unconnected] SamsonWorker > ";
  }

  std::ostringstream output;
  if (data_model_ != NULL) {
    size_t commit_id = data_model_->getCurrentModel()->current_data().commit_id();
    if (commit_id > 0) {
      output << "[ DM Commit " << (commit_id - 1) << " ]";
    }
  }

  if (network_ != NULL) {
    output << "[ " << network_->getClusterConnectionStr().c_str() << " ]";
  }

  output << au::str(" SamsonWorker %lu > ", worker_id);

  return output.str();
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetWorkerAllLogChannels(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());

  collection->set_name("wlog channels");

  for (int i = 0; i < au::log_central->log_channels().num_channels(); ++i) {
    gpb::CollectionRecord *record = collection->add_record();
    std::string name = au::log_central->log_channels().channel_name(i);
    std::string description = au::log_central->log_channels().channel_description(i);

    ::samson::add(record, "Channel", name, "different,left");

    if (visualization.get_flag("-hits")) {
      ::samson::add(record, "Hits", au::log_central->log_channels_filter().GetHitDescriptionForChannel(
                      i), "different,left");
    }

    ::samson::add(record, "Description", description, "different,left");
  }
  return collection;
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetWorkerLogStatus(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());

  collection->set_name("wlog status");
  gpb::CollectionRecord *record = collection->add_record();

  ::samson::add(record, "log server", au::log_central->GetPluginStatus("server"), "different");
  ::samson::add(record, "Channels", au::log_central->GetPluginChannels("server"), "different");

  return collection;
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetKVRangesCollection(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_title("KVRanges collection");
  collection->set_name("kv_ranges");

  // Get current version of the cluster
  au::SharedPointer<samson::gpb::ClusterInfo> cluster_info = worker_controller_->GetCurrentClusterInfo();

  // Get current data model
  au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();

  if ((cluster_info == NULL) || (data_model == NULL)) {
    return collection;
  }

  for (int i = 0; i < cluster_info->process_units_size(); ++i) {
    gpb::CollectionRecord *record = collection->add_record();

    KVRange range(cluster_info->process_units(i).hg_begin(), cluster_info->process_units(i).hg_end());

    add(record, "KVRange", range.str(), "different");
    add(record, "Worker", cluster_info->process_units(i).worker_id(), "different");

    std::ostringstream replica_workers;
    for (int r = 0; r < cluster_info->process_units(i).replica_worker_id_size(); ++r) {
      replica_workers << cluster_info->process_units(i).replica_worker_id(r) << " ";
    }
    add(record, "Worker replicas", replica_workers.str(), "different");

    // Information about data for this range
    BlockKVInfo info;
    BlockKVInfo max_info;
    const gpb::Data& data = data_model->current_data();
    for (int q = 0; q < data.queue_size(); ++q) {
      BlockKVInfo queue_info = gpb::getBlockKVInfoForQueue(data.queue(q), range);

      info.Append(queue_info);
      if (queue_info.size > max_info.size) {
        max_info = queue_info;
      }
    }

    add(record, "KVInfo", info.str(), "different");
    add(record, "Max KVInfo per queue", max_info.str(), "different");
  }

  return collection;
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetWorkerCollection(const Visualization& visualization) {
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());

  collection->set_name("workers");

  gpb::CollectionRecord *record = collection->add_record();


  // Common fields
  ::samson::add(record, "Status", state_message_, "different");

  if (visualization.get_flag("engine")) {
    size_t num_elements = engine::Engine::shared()->GetNumElementsInEngineStack();
    double waiting_time = engine::Engine::shared()->GetMaxWaitingTimeInEngineStack();
    size_t num_buffers = engine::Engine::memory_manager()->num_buffers();
    ::samson::add(record, "#buffers in memory", num_buffers, "f=uint64,sum");
    ::samson::add(record, "#elements in engine", num_elements, "f=uint64,sum");
    ::samson::add(record, "Max waiting time", waiting_time, "f=double,different");
  } else if (visualization.get_flag("disk")) {
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
    ::samson::add(record, "BM writing", stream::BlockManager::shared()->scheduled_write_size(), "f=uint64,sum");
    ::samson::add(record, "BM reading", stream::BlockManager::shared()->scheduled_read_size(), "f=uint64,sum");
    double usage =  engine::Engine::disk_manager()->on_off_activity();
    ::samson::add(record, "Disk usage", au::str_percentage(usage), "differet");
  } else if (visualization.get_flag("modules")) {
    if (!modules_available_) {
      ::samson::add(record, "Modules", "No", "different");
    } else if (last_modules_version_ != SIZE_T_UNDEFINED) {
      ::samson::add(record, "Modules", au::str("Yes (commit %lu)", last_modules_version_), "different");
    } else {
      ::samson::add(record, "Modules", "No modules", "different");
    }
  } else if (visualization.get_flag("traffic")) {
    ::samson::add(record, "#Disk ops", engine::Engine::disk_manager()->num_disk_operations(), "f=uint64,sum");
    ::samson::add(record, "Disk in B/s", engine::Engine::disk_manager()->rate_in(), "f=uint64,sum");
    ::samson::add(record, "Disk out B/s", engine::Engine::disk_manager()->rate_out(), "f=uint64,sum");
    ::samson::add(record, "Net in B/s", network_->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "Net out B/s", network_->get_rate_out(), "f=uint64,sum");
    ::samson::add(record, "ZK in B/s", zoo_connection_->get_rate_in(), "f=uint64,sum");
    ::samson::add(record, "ZK out B/s", zoo_connection_->get_rate_out(), "f=uint64,sum");
  } else if (visualization.get_flag("data_model")) {
    ::samson::add(record, "DataModel", worker_controller_->GetMyLastCommitId(), "different");
  } else {
    ::samson::add(record, "Mem used", engine::Engine::memory_manager()->used_memory(), "f=uint64,sum");
    ::samson::add(record, "Mem total", engine::Engine::memory_manager()->memory(), "f=uint64,sum");
    ::samson::add(record, "Cores used", engine::Engine::process_manager()->num_used_procesors(), "f=uint64,sum");
    ::samson::add(record, "Cores total", engine::Engine::process_manager()->max_num_procesors(), "f=uint64,sum");
  }

  return collection;
}

au::SharedPointer<au::zoo::Connection> SamsonWorker::zoo_connection() {
  return zoo_connection_;
}

au::SharedPointer<SamsonWorkerController> SamsonWorker::worker_controller() {
  return worker_controller_;
}

au::SharedPointer<DataModel> SamsonWorker::data_model() {
  return data_model_;
}

au::SharedPointer<WorkerNetwork> SamsonWorker::network() {
  return network_;
}

au::SharedPointer<SamsonWorkerRest> SamsonWorker::samson_worker_rest() {
  return samson_worker_rest_;
}

au::SharedPointer<WorkerBlockManager> SamsonWorker::worker_block_manager() {
  return worker_block_manager_;
}

au::SharedPointer<stream::WorkerTaskManager> SamsonWorker::task_manager() {
  return task_manager_;
}

au::SharedPointer<WorkerCommandManager> SamsonWorker::workerCommandManager() {
  return workerCommandManager_;
}

void SamsonWorker::ReloadModulesIfNecessary() {
  // Recover current data model
  au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();
  gpb::Data *data = data_model->mutable_current_data();

  // Get .modules queue
  gpb::Queue *queue = get_queue(data, ".modules");

  // If no .modules queue, remove local directory and clear ModuleManager
  if (!queue) {
    // Clear modules
    au::Singleton<ModulesManager>::shared()->ClearModulesManager();
    last_modules_version_ = SIZE_T_UNDEFINED;
    modules_available_ = true;
    return;
  }

  // Check if we have all necessary blocks
  int missing_blocks = 0;
  for (int b = 0; b < queue->blocks_size(); ++b) {
    size_t block_id = queue->blocks(b).block_id();
    if (stream::BlockManager::shared()->GetBlock(block_id) == NULL) {
      // Add this block to be requested to other workers
      worker_block_manager_->RequestBlock(block_id);
      LOG_V(logs.modules_manager, ("Missing block(%lu) detected and requested, pos(%d)", block_id, b));
      ++missing_blocks;
    }
  }

  if (missing_blocks > 0) {
    modules_available_ = false;
    last_modules_version_ = SIZE_T_UNDEFINED;
    LOG_W(logs.modules_manager, ("Not possible to load modules because there are %d missing_blocks", missing_blocks));
    au::Singleton<ModulesManager>::shared()->ClearModulesManager();
    return;
  }

  modules_available_ = true;
  size_t commit_id = queue->commit_id();
  if ((last_modules_version_ != SIZE_T_UNDEFINED) && (commit_id <= last_modules_version_)) {
    LOG_V(logs.modules_manager,
          ("Not necessary to reaload modules since we have the last version %lu", last_modules_version_));
    return;     // Not necessary to update since we have the last version
  }

  // Update this version
  last_modules_version_ = commit_id;

  // Clear modules
  au::Singleton<ModulesManager>::shared()->ClearModulesManager();

  // Clear directory
  au::ErrorManager error_rm_directory;
  std::string directory = au::Singleton<SamsonSetup>::shared()->worker_modules_directory();
  au::RemoveDirectory(directory, error_rm_directory);
  au::CreateDirectory(directory);

  // Write each loaded file
  for (int i = 0; i < queue->blocks_size(); ++i) {
    size_t block_id = queue->blocks(i).block_id();

    // Recover block
    stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id);

    if (block == NULL) {
      LOG_SW(("Block %lu necessary for a module not found. Skipping..."));
      continue;
    }

    engine::BufferPointer buffer = block->buffer();
    if (buffer == NULL) {
      buffer = block->GetBufferFromDisk();
    }
    if (buffer == NULL) {
      LOG_SW(("Error reading module from block %lu file %s", block->block_id(), block->file_name().c_str()));
      continue;
    }

    std::string target_file_name = au::str("%s/module_%05d", directory.c_str(), i);
    FILE *target_file = fopen(target_file_name.c_str(), "w");
    fwrite(buffer->data() + sizeof(KVHeader), buffer->size() - sizeof(KVHeader), 1, target_file);
    fclose(target_file);
  }

  // Reload modules from this directory
  au::ErrorManager error;
  au::Singleton<ModulesManager>::shared()->AddModulesFromDirectory(directory, error);
  if (error.HasErrors()) {
    LOG_W(logs.worker, ("Error reloading modules: %s", error.GetLastError().c_str()));
  }
}

bool SamsonWorker::IsReady() {
  return (state_ == ready);
}

bool SamsonWorker::IsConnected() {
  return state_ != unconnected;
}

void SamsonWorker::fill(gpb::CollectionRecord *record
                        , const std::string& name
                        , gpb::Data *data
                        , const Visualization& visualization) {
  ::samson::add(record, "name", name, "different");
  FullKVInfo info = gpb::GetFullKVInfo(data);
  ::samson::add(record, "ZK size", data->ByteSize(), "uint64,different");
  ::samson::add(record, "data", info.str(), "different");
  ::samson::add(record, "#commit", data->commit_id(), "different");
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetModulesCollection(const Visualization& visualization) {
  // Get a copy of the current version
  au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();

  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("modules");

  gpb::Queue *queue = gpb::get_queue(data_model->mutable_current_data(), ".modules");

  // Create a tmp directory
  std::string directory = au::GetRandomTmpFileOrDirectory();
  au::CreateDirectory(directory);

  // Set of names used so far to detect colision name...
  std::set<std::string> module_names;

  if (queue) {
    for (int i = 0; i < queue->blocks_size(); i++) {
      size_t block_id = queue->blocks(i).block_id();

      gpb::CollectionRecord *record = collection->add_record();
      ::samson::add(record, "block_id", str_block_id(block_id), "different");

      stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id);
      if (block == NULL) {
        ::samson::add(record, "description", "", "left,diferent");
        ::samson::add(record, "error", "Block not found in this worker. Waiting for it!", "left,different");
        continue;
      }

      // Save to a temporal file
      engine::BufferPointer buffer = block->buffer();
      if (buffer == NULL) {
        buffer = block->GetBufferFromDisk();
      }
      if (buffer == NULL) {
        ::samson::add(record, "description", "", "left,diferent");
        ::samson::add(record, "error", "Not possible to load content of this block", "left,different");
        continue;
      }

      std::string target_file_name = au::str("%s/modules_%05d", directory.c_str(), i);
      FILE *target_file = fopen(target_file_name.c_str(), "w");
      fwrite(buffer->data() + sizeof(KVHeader), buffer->size() - sizeof(KVHeader), 1, target_file);
      fclose(target_file);

      au::ErrorManager error;
      Module *module = ModulesManager::LoadModule(target_file_name, error);

      if (error.HasErrors()) {
        ::samson::add(record, "description", "", "left,diferent");
        ::samson::add(record, "error", au::str("Error: %s", error.GetLastError().c_str()), "left,different");
        continue;
      }

      std::string description = au::str("Module %s / %lu operations / %lu datas"
                                        , module->name.c_str()
                                        , module->operations.size()
                                        , module->datas.size());
      ::samson::add(record, "description", description, "left,diferent");

      if (module_names.find(module->name) != module_names.end()) {
        ::samson::add(record, "error", "Previous module with the same name", "left,diferent");
      } else {
        ::samson::add(record, "error", "", "left,diferent");
        module_names.insert(module->name);
      }
      delete module;
    }
  }

  au::ErrorManager error;
  au::RemoveDirectory(directory, error);

  return collection;
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetCollectionForDataModelStatus(const Visualization& visualization) {
  // Get a copy of the current version
  au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();

  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("data_model");

  // Frozen data model
  fill(collection->add_record(), "frozen", data_model->mutable_previous_data(), visualization);

  // Candidate
  if (data_model->has_candidate_data()) {
    fill(collection->add_record(), "candidate", data_model->mutable_candidate_data(), visualization);
  }

  // Current data model
  fill(collection->add_record(), "current", data_model->mutable_current_data(), visualization);

  return collection;
}

au::SharedPointer<gpb::Collection> SamsonWorker::GetCollectionForDataModelCommits(const Visualization& visualization) {
  // Get a copy of the current version
  au::SharedPointer<gpb::DataModel> data_model = data_model_->getCurrentModel();

  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("data_model_commits");

  for (int i = 0; i < data_model->commit_size(); ++i) {
    gpb::CollectionRecord *record = collection->add_record();
    ::samson::add(record, "commit_id", data_model->commit(i).id(), "different");
    ::samson::add(record, "message", data_model->commit(i).message(), "different");
  }

  return collection;
}

au::SharedPointer<GlobalBlockSortInfo> SamsonWorker::GetGlobalBlockSortInfo() {
  if ((worker_controller_ == NULL) || (data_model_ == NULL)) {
    return au::SharedPointer<GlobalBlockSortInfo>(NULL);
  }

  au::SharedPointer<GlobalBlockSortInfo> blocks_sort_info(new GlobalBlockSortInfo());

  // Update with task manager
  task_manager_->Update(blocks_sort_info.shared_object());

  // Get ranges of this worker
  std::vector<KVRange> ranges = worker_controller_->GetMyKVRanges();

  // Update with Data model information
  au::SharedPointer<gpb::DataModel> data = data_model_->getCurrentModel();
  if (data != NULL) {
    const gpb::Data& current_data =  data->current_data();   // Get current data model
    for (int q = 0; q < current_data.queue_size(); q++) {
      const gpb::Queue& queue = current_data.queue(q);
      for (int b = 0; b < queue.blocks_size(); b++) {
        blocks_sort_info->NotifyQueue(queue.blocks(b).block_id(), queue.name());
      }
    }

    // Past data models
    for (int q = 0; q < data->previous_data().queue_size(); q++) {
      const gpb::Queue& queue = data->previous_data().queue(q);
      for (int b = 0; b < queue.blocks_size(); b++) {
        blocks_sort_info->NotifyQueueInPreviousDataModel(queue.blocks(b).block_id(), queue.name());
      }
    }
    if (data->has_candidate_data()) {
      for (int q = 0; q < data->candidate_data().queue_size(); q++) {
        const gpb::Queue& queue = data->candidate_data().queue(q);
        for (int b = 0; b < queue.blocks_size(); b++) {
          blocks_sort_info->NotifyQueueInPreviousDataModel(queue.blocks(b).block_id(), queue.name());
        }
      }
    }

    // Inform about stream operations in ther worker or in exterior workers
    for (size_t r = 0; r < ranges.size(); r++) {
      for (int o = 0; o < current_data.operations_size(); o++) {
        std::string name = au::str("%s %s", current_data.operations(o).name().c_str(), ranges[r].str().c_str());

        for (int i = 0; i < current_data.operations(o).inputs_size(); i++) {
          std::string queue_name = current_data.operations(o).inputs(i);
          // Consider only blocks of data for this worker
          gpb::Queue *queue = gpb::get_queue(data->mutable_current_data(), queue_name);
          if (!queue) {
            continue;
          }

          bool state = false;
          if (current_data.operations(o).inputs_size() > 1) {
            if (!current_data.operations(o).batch_operation()) {
              if (!current_data.operations(o).reduce_forward()) {
                if (i == (current_data.operations(o).inputs_size() - 1)) {
                  state = true;
                }
              }
            }
          }

          size_t total = 0;  // Total accumulated size for stream operation in this worker
          size_t extern_total = 0;  // Total accumuated size for strema operations in other workers
          for (int b = 0; b < queue->blocks_size(); b++) {
            const gpb::Block& block = queue->blocks(b);
            KVRange range = block.range();
            if (range.IsOverlapped(ranges[r])) {
              blocks_sort_info->NotifyInputForStreamOperation(block.block_id()
                                                              , name
                                                              , queue_name
                                                              , state
                                                              , total);
              total += block.size();
            } else {
              blocks_sort_info->NotifyInputForExternStreamOperation(block.block_id()
                                                                    , name
                                                                    , queue_name
                                                                    , extern_total);
              extern_total += block.size();
            }
          }
        }
      }
    }
  }

  return blocks_sort_info;
}

bool SamsonWorker::IsWorkerReadyForBlockRequest(size_t worker_id) {
  // Reject for memory ?
  double mem_usage = engine::Engine::memory_manager()->memory_usage();

  if (mem_usage >= 1.0) {
    LOG_W(logs.worker, ("Rejecting block request since memory usage is %s", au::str_percentage(mem_usage).c_str()));
    return false;
  }

  // Reject for output network queues
  size_t memory = engine::Engine::memory_manager()->memory();
  size_t all_queue_size = network_->GetAllQueuesSize();
  if (all_queue_size > (0.25 * (double)memory)) {
    LOG_W(logs.worker, ("Rejecting block request: Size of all output-queue %s", au::str(all_queue_size).c_str()));
    return false;
  }

  size_t queue_size = network_->GetQueueSizeForWorker(worker_id);
  if (queue_size > (0.2 * (double)memory)) {
    LOG_W(logs.worker, ("Rejecting block request: Size of output-queue %s", au::str(queue_size).c_str()));
    return false;
  }

  // If number of scheduled block_request task is Excessive, also stop
  // TODO

  return true;
}
}

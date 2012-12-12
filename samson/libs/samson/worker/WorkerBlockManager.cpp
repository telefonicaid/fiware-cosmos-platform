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
#include "samson/worker/WorkerBlockManager.h"  // Own interface

#include <utility>    // std::pair<>

#include "au/string/StringUtilities.h"
#include "engine/Engine.h"
#include "engine/Notification.h"
#include "samson/common/Logs.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
void WorkerBlockManager::Review() {
  LOG_D(logs.worker_block_manager, ("Review"));

  // Review all block requests
  {
    au::map<size_t, BlockRequest>::iterator it;
    for (it = block_requests_.begin(); it != block_requests_.end(); ) {
      it->second->Review();

      // Remove if necessary...
      if (it->second->finished()) {
        delete it->second;
        block_requests_.erase(it++);
      } else {
        ++it;
      }
    }
  }


  // Review all defrag operations
  {
    std::vector<std::string> defrag_tasks_names = defrag_tasks_.getKeysVector();
    std::vector<std::string> defrag_tasks_names_remove;
    for (size_t i = 0; i < defrag_tasks_names.size(); i++) {
      std::string defrag_task_name = defrag_tasks_names[i];

      au::SharedPointer<stream::DefragTask> defrag_task = defrag_tasks_.Get(defrag_task_name);

      if (defrag_task == NULL) {
        continue;
      }

      // If we are running a task, let see if it is finished
      if (defrag_task->IsWorkerTaskFinished()) {
        LOG_V(logs.worker_block_manager, ("Defrag task %s has finish", defrag_task_name.c_str()));

        defrag_tasks_names_remove.push_back(defrag_task_name);     // add key to be removed latter

        if (defrag_task->error().HasErrors()) {
          std::string error_message = defrag_task->error().GetLastError();
          LOG_W(logs.worker_block_manager, ("Error in defrag task %lu:%s (%s)"
                                            , defrag_task->id()
                                            , defrag_task_name.c_str()
                                            , error_message.c_str()));
        } else {
          // Commit changes and release task
          LOG_V(logs.worker_block_manager, ("Commiting defrag task %lu:%s", defrag_task->id(), defrag_task->str().c_str()));

          std::string commit_command = defrag_task->commit_command();
          std::string caller = au::str("defrag task %lu // %s", defrag_task->worker_task_id(), defrag_task_name.c_str());
          au::ErrorManager error;
          samson_worker_->data_model()->Commit(caller, commit_command, error);

          if (error.HasErrors()) {
            LOG_W(logs.worker_block_manager, ("Error commiting defrag task %lu:%s (%s)"
                                              , defrag_task->id()
                                              , defrag_task_name.c_str()
                                              , error.GetLastError().c_str()));
          }
        }
      }
    }

    // Remove finished tasks
    for (size_t i = 0; i < defrag_tasks_names_remove.size(); i++) {
      defrag_tasks_.Extract(defrag_tasks_names_remove[i]);
    }
  }
}

size_t WorkerBlockManager::CreateBlock(engine::BufferPointer buffer) {
  // Get a new id for this block ( identifiers are unique cluster-wide )
  size_t block_id = samson_worker_->worker_controller()->get_new_block_id();

  // Add to the block manager
  stream::BlockManager::shared()->CreateBlock(block_id, buffer);

  LOG_V(logs.worker_block_manager, ("Create block from buffer %s --> %s"
                                    , buffer->str().c_str()
                                    , str_block_id(block_id).c_str()));

  return block_id;  // Return the identifier of the new block
}

// Messages received from other workers
void WorkerBlockManager::ReceivedBlockRequestResponse(size_t block_id, size_t worker_id) {
  LOG_V(logs.worker_block_manager, ("ReceivedBlockRequestResponse for %s ( worker %lu )"
                                    , str_block_id(block_id).c_str()
                                    , worker_id));

  BlockRequest *block_request = block_requests_.extractFromMap(block_id);
  if (block_request) {
    delete block_request;
  }
}

void WorkerBlockManager::ReceivedBlockRequestResponse(size_t block_id, size_t worker_id,
                                                      const std::string& error_message) {
  LOG_V(logs.worker_block_manager, ("ReceivedBlockRequestResponse for %s ( worker %lu error %s)"
                                    , str_block_id(block_id).c_str()
                                    , worker_id
                                    , error_message.c_str()));

  BlockRequest *block_request = block_requests_.findInMap(block_id);
  if (block_request) {
    block_request->NotifyErrorMessage(worker_id, error_message);
  }
}

au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForBlockRequests(const Visualization& visualization)
{
  return GetCollectionForMap("block_requests", block_requests_, visualization);
}

au::SharedPointer<gpb::Collection> WorkerBlockManager::GetCollectionForBlockDefrags(const Visualization& visualization)
{
  // Create a new collection to be returned
  au::SharedPointer<gpb::Collection> collection(new gpb::Collection());
  collection->set_name("block_requests");

  std::map<std::string, au::SharedPointer< stream::DefragTask > >::iterator iter;

  for (iter = defrag_tasks_.begin(); iter != defrag_tasks_.end(); ++iter) {
    // Get pointer to the instance
    au::SharedPointer<stream::DefragTask> defrag_task = iter->second;

    // Create a new record for this instance
    gpb::CollectionRecord *record = collection->add_record();

    add(record, "name", iter->first, "left,different");
    add(record, "time", au::str_time_simple(defrag_task->GetTotalTime()), "different");
    add(record, "info", defrag_task->str(), "left,different");
  }

  return collection;
}

void WorkerBlockManager::RequestBlock(size_t block_id) {
  BlockRequest *block_request = block_requests_.findInMap(block_id);

  if (block_request) {
    LOG_D(logs.worker_block_manager, ("Requested block %s.... Already requested!", str_block_id(block_id).c_str()));
    return;   // already requested
  }

  if (block_requests_.findInMap(block_id)) {
    LOG_D(logs.worker_block_manager,
          ("Requested block %s.... previously requestes and still waiting!", str_block_id(block_id).c_str()));
    return;
  }

  // New block request for this block
  LOG_V(logs.worker_block_manager, ("Requested block %s", str_block_id(block_id).c_str()));
  block_request = new BlockRequest(samson_worker_, block_id);
  block_requests_.insertInMap(block_id, block_request);
}

void WorkerBlockManager::RequestBlocks(const std::set<size_t>& pending_block_ids) {
  std::set<size_t>::const_iterator it;
  for (it = pending_block_ids.begin(); it != pending_block_ids.end(); ++it) {
    size_t block_id = *it;        // Identifier of the block
    RequestBlock(block_id);
  }
}

void WorkerBlockManager::Reset() {
  LOG_V(logs.worker_block_manager, ("Reset"));
  // Remove all internal elements
  block_requests_.clearMap();
}

void WorkerBlockManager::AddBlockBreak(const std::string& queue_name, size_t block_id,
                                       const std::vector<KVRange>& ranges) {
  // Name in the map
  std::string name = au::str("%s_%s", queue_name.c_str(), au::str(block_id).c_str());

  // If this operation is already scheduled, do nothing
  if (defrag_tasks_.Get(name) != NULL) {
    return;
  }

  LOG_V(logs.worker_block_manager,
        ("AddBlockBreak for block %s in queeu %s", str_block_id(block_id).c_str(), queue_name.c_str()));

  // Get the block to de defrag
  stream::BlockPointer block = stream::BlockManager::shared()->GetBlock(block_id);
  if (block == NULL) {
    LOG_W(logs.worker_block_manager, ("BLock %s not found to be defrag. Ignoring...", str_block_id(block_id).c_str()));
    return;
  }

  // Create a worker task and schedule
  size_t task_id = samson_worker_->task_manager()->getNewId();
  au::SharedPointer<stream::DefragTask> defrag_task;
  defrag_task.Reset(new stream::DefragTask(samson_worker_, queue_name, task_id, ranges));

  // Insert in the local map of tasks
  defrag_tasks_.Set(name, defrag_task);

  // Add a unique block id
  defrag_task->AddInput(0, block, block->getKVRange(), block->getKVInfo());

  // Schedule task
  samson_worker_->task_manager()->Add(defrag_task.dynamic_pointer_cast<stream::WorkerTaskBase>());
}

// Received a message from a delilah
void WorkerBlockManager::ReceivedPushBlock(size_t delilah_id
                                           , size_t push_id
                                           , engine::BufferPointer buffer
                                           , const std::vector<std::string>& queues) {
  LOG_V(logs.worker_block_manager, ("Received a push block (Delilah %s PushId %lu Buffer %s Queues %s)"
                                    , au::code64_str(delilah_id).c_str()
                                    , push_id
                                    , buffer->str().c_str()
                                    , au::str(queues).c_str()));

  if (buffer == NULL) {
    LOG_W(logs.worker_block_manager, ("Push message without a buffer. This is an error..."));
    SendPushBlockResponseWithError(delilah_id, push_id, "No buffer provided");
    return;
  }

  // Check valid header size
  size_t block_size = buffer->size();
  if (buffer->size() < sizeof(KVHeader)) {
    LOG_W(logs.worker_block_manager, ("Push message with a non-valid buffer.Ignoring..."));
    SendPushBlockResponseWithError(delilah_id, push_id, "Invalid buffer provided");
    return;
  }

  KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
  if (!header->Check() || !header->range.isValid()) {
    LOG_W(logs.worker_block_manager, ("Push message with an invalid buffer. Ignoring..."));
    SendPushBlockResponseWithError(delilah_id, push_id, "Invalid buffer provided");
    return;
  }

  if (header->IsTxt()) {
    // Random hash-group based on all my ranges
    std::vector<KVRange> ranges = samson_worker_->worker_controller()->GetMyKVRanges();
    std::vector<size_t> all_hgs;
    for (size_t i = 0; i < ranges.size(); i++) {
      for (int j = ranges[i].hg_begin_; j < ranges[i].hg_end_; j++) {
        all_hgs.push_back(j);
      }
    }

    if (all_hgs.size() == 0) {
      LOG_W(logs.worker_block_manager, ("Push message rejected. No hash group assigned to me"));
      SendPushBlockResponseWithError(delilah_id, push_id, "Internal error in this worker");
      return;
    }

    int hg =  all_hgs[ rand() % all_hgs.size()];
    header->range.set(hg, hg + 1);
  } else if (header->IsModule()) {
    header->range.set(0, KVFILE_NUM_HASHGROUPS);        // Make sure it is full range
  } else {
    LOG_W(logs.worker_block_manager, ("Push message with a buffer that is not data or a module.Ignoring..."));
    SendPushBlockResponseWithError(delilah_id, push_id, "Invalid buffer provided");
    return;
  }

  // Create a new block in this worker ( and start distribution process )
  size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);

  if (block_id == static_cast<size_t>(-1)) {
    LOG_W(logs.worker_block_manager, ("Error creating block in a push operation ( block_id -1 )"));
    SendPushBlockResponseWithError(delilah_id, push_id, "Internal error in this worker");
    return;
  }

  // Get commit command for this push operation
  CommitCommand commit_command;
  for (size_t i = 0; i < queues.size(); i++) {
    commit_command.AddBlock(queues[i], block_id, block_size, header->GetKVFormat(), header->range, header->info);
  }

  // Commit to data model
  std::string command = commit_command.GetCommitCommand();
  au::ErrorManager error;
  std::string caller = au::str("PushOperation from delilah %s ( push id %lu )"
                               , au::code64_str(delilah_id).c_str()
                               , push_id);
  au::SharedPointer<gpb::DataModel> data_model = samson_worker_->data_model()->Commit(caller, command, error);

  if (error.HasErrors()) {
    LOG_W(logs.worker_block_manager, ("Error comitting a push operation to data model: %s", error.GetLastError().c_str()));
    SendPushBlockResponseWithError(delilah_id, push_id, "Internal error in this worker");
    return;
  }

  // If no delilah waiting, no message is sent
  if (delilah_id == SIZE_T_UNDEFINED) {
    return;
  }

  if (samson_worker_->worker_controller()->IsSingleWorkerCluster()) {
    SendPushBlockResponse(delilah_id, push_id);
    SendPushBlockConfirmation(delilah_id, push_id);
  } else {
    // Add in the list of push items to be confirmed
    size_t commit_id = data_model->current_data().commit_id();
    push_items_.push_back(WorkerPushItem(delilah_id, push_id, commit_id));

    // Send a message to delilah to inform we have received correctly
    SendPushBlockResponse(delilah_id, push_id);
  }
}

void WorkerBlockManager::ReviewPushItems(size_t previous_data_commit_id, size_t current_data_commit_id) {
  std::list<WorkerPushItem>::iterator iter;
  for (iter = push_items_.begin(); iter != push_items_.end(); ) {
    if (iter->commit_id() <= previous_data_commit_id) {
      SendPushBlockConfirmation(iter->delilah_id(), iter->push_id());            // Operation can be confirmed
      iter = push_items_.erase(iter);
      continue;
    }

    if (iter->commit_id() > current_data_commit_id) {
      std::string error_message = au::str("Canceled since commit# %lu is newer than current commit# %lu "
                                          , iter->commit_id()
                                          , current_data_commit_id);
      SendPushBlockConfirmationWithError(iter->delilah_id(), iter->push_id(), error_message);
      iter = push_items_.erase(iter);
      continue;
    }

    ++iter;
  }
}

void WorkerBlockManager::RemoveRequestIfNecessary(const std::set<size_t>& all_block_ids) {
  au::map<size_t, BlockRequest>::iterator iter;
  for (iter = block_requests_.begin(); iter != block_requests_.end(); ) {
    if (all_block_ids.find(iter->second->block_id()) == all_block_ids.end()) {
      delete iter->second;  // Remove the request itself
      block_requests_.erase(iter++);          // Remove this request
    } else {
      ++iter;   // keep request
    }
  }
}

void WorkerBlockManager::SendPushBlockResponse(size_t delilah_id, size_t push_id) {
  if (delilah_id == SIZE_T_UNDEFINED) {
    return;
  }
  PacketPointer packet(new Packet(Message::PushBlockResponse));
  packet->to = NodeIdentifier(DelilahNode, delilah_id);
  packet->message->set_push_id(push_id);
  samson_worker_->network()->Send(packet);
}

void WorkerBlockManager::SendPushBlockResponseWithError(size_t delilah_id, size_t push_id, const std::string& error) {
  if (delilah_id == SIZE_T_UNDEFINED) {
    return;
  }
  PacketPointer packet(new Packet(Message::PushBlockResponse));
  packet->to = NodeIdentifier(DelilahNode, delilah_id);
  packet->message->set_push_id(push_id);
  packet->message->mutable_error()->set_message(error);
  samson_worker_->network()->Send(packet);
}

void WorkerBlockManager::SendPushBlockConfirmation(size_t delilah_id, size_t push_id) {
  if (delilah_id == SIZE_T_UNDEFINED) {
    return;
  }
  PacketPointer packet(new Packet(Message::PushBlockConfirmation));
  packet->to = NodeIdentifier(DelilahNode, delilah_id);
  packet->message->set_push_id(push_id);
  samson_worker_->network()->Send(packet);
}

void WorkerBlockManager::SendPushBlockConfirmationWithError(size_t delilah_id, size_t push_id,
                                                            const std::string& error) {
  PacketPointer packet(new Packet(Message::PushBlockConfirmation));

  packet->to = NodeIdentifier(DelilahNode, delilah_id);
  packet->message->set_push_id(push_id);
  packet->message->mutable_error()->set_message(error);
  samson_worker_->network()->Send(packet);
}
}

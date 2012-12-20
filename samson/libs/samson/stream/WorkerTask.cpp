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

#include "samson/stream/WorkerTask.h"      // Own interface

#include "samson/common/KVInputVector.h"
#include "samson/common/Logs.h"
#include "samson/common/coding.h"
#include "samson/stream/Block.h"              // samson::Stream::Block
#include "samson/stream/BlockManager.h"       // samson::stream::BlockManager
#include "samson/stream/BlockReaderCollection.h"
#include "samson/stream/ProcessWriter.h"
#include "samson/worker/CommitCommand.h"
#include "samson/worker/SamsonWorker.h"

namespace samson {
namespace stream {
// Hand functiontion to get the type of ProcessIsolated based on the operation type
ProcessIsolated::ProcessBaseType get_type(Operation *operation) {
  switch (operation->getType()) {
    case Operation::parserOut:
    case Operation::parserOutReduce:
      return ProcessIsolated::txt;

    default:
      return ProcessIsolated::key_value;
  }
}

WorkerTask::WorkerTask(SamsonWorker *samson_worker
                       , size_t id
                       , const gpb::StreamOperation& stream_operation
                       , Operation *operation
                       , KVRange range
                       ) :
  ProcessIsolated(samson_worker
                  , id
                  , stream_operation.operation().c_str()
                  , au::str("WorkerTask %lu: %s", id, stream_operation.operation().c_str()), get_type(operation)
                  ) {
  // Keep a pointer to samson_worker to create output blocks
  samson_worker_ = samson_worker;

  // Some environment variables
  set_process_item_description(stream_operation.operation());

  // Set output formats for this operation
  addOutputsForOperation(operation);

  // Keep a pointer to the operation
  operation_ = operation;

  // keep the working range
  range_ = range;

  // Copy information about stream operation to be executed
  stream_operation_ = new gpb::StreamOperation();
  stream_operation_->CopyFrom(stream_operation);

  // Add a new output queue to emit big-data traces ( system.String , system.Void )
  stream_operation_->add_outputs("debug_" + stream_operation_->name());

  // Channels state and num input channels to collect statistics at the end of the task
  if (operation_ && stream_operation_) {
    if (operation_->getType() == Operation::reduce) {
      num_input_channels_ = operation_->getNumInputs();
      if (stream_operation_->batch_operation()) {
        state_input_channel_ = -1;
      } else {
        state_input_channel_ = num_input_channels_ - 1;
        num_input_channels_--;
      }
    } else {
      state_input_channel_ = -1;   // No state
      num_input_channels_ = 1;   // Only one input for all operations ( map, parse, parseout, etc...)
    }
  } else {
    // No data to be collected
    state_input_channel_ = -1;
    num_input_channels_ = 0;
  }


  LOG_M(logs.worker_task, ("Worker task created WT%s", str().c_str()));
}

WorkerTask::~WorkerTask() {
  LOG_M(logs.worker_task, ("Worker task destroyed WT%s", str().c_str()));
  delete stream_operation_;
}

std::vector<size_t> WorkerTask::ProcessOutputBuffers() {
  std::vector<size_t> new_block_ids;

  engine::BufferPointer buffer = GetNextOutputBuffer();

  while (buffer != NULL) {
    // Output is in the environemnt variables
    int output = buffer->environment().Get("output", -10);

    if (output == -10) {
      LM_X(1, ("Internal error"));
    }

    // Information for generated block
    KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());

    // Create a block ( and distribute it )
    size_t block_id = samson_worker_->worker_block_manager()->CreateBlock(buffer);
    BlockPointer block = BlockManager::shared()->GetBlock(block_id);

    // Add output to this operation
    AddOutput(output, block, header->range, header->info);

    LOG_M(logs.worker_task, ("Task WT%lu: Generated block %s ( %s ) for output %d "
                             , id()
                             , str_block_id(block_id).c_str()
                             , header->str().c_str()
                             , output));

    new_block_ids.push_back(block_id);

    // Extract the next output buffer
    buffer = GetNextOutputBuffer();
  }

  return new_block_ids;
}

std::string WorkerTask::commit_command() {
  // Do not remove last input in reduce_forward operation
  // Not that in normal stream operation, last input should be removed since new state is added at the output
  bool remove_last_input = true;

  if (stream_operation_->has_reduce_forward() && stream_operation_->reduce_forward()) {
    remove_last_input = false;
  }

  std::vector<std::string> inputs;
  std::vector<std::string> outputs;

  for (int c = 0; c < stream_operation_->inputs_size(); ++c) {
    if (!remove_last_input && (c == (stream_operation_->inputs_size() - 1))) {
      continue;  // Skip this last input
    }
    inputs.push_back(stream_operation_->inputs(c));
  }
  for (int c = 0; c < stream_operation_->outputs_size(); ++c) {
    outputs.push_back(stream_operation_->outputs(c));
  }

  return generate_commit_command(inputs, outputs);
}

void WorkerTask::initProcessIsolated() {
  LOG_M(logs.background_process, ("Init background process for task WT%lu", id()));
  // Review input blocks to count key-values
  block_list_container_.Review(error_);
}

void WorkerTask::generateKeyValues(samson::ProcessWriter *writer) {
  au::Cronometer cronometer;

  LOG_M(logs.background_process, ("Generate key-values for task WT%lu", id()));

  // Get KVFiles scaning input data
  block_list_container_.ReviewKVFile(error_);
  if (error_.HasErrors()) {
    LOG_E(logs.background_process, ("Error generating key-values for task WT%lu, block_id:'%s'", id(), block_list_container_.str_block_ids().c_str()));
    setUserError(error_.GetLastError());
    return;
  }

  switch (operation_->getType()) {
    case Operation::parser:
      generateKeyValues_parser(writer);
      break;

    case Operation::map:
      generateKeyValues_map(writer);
      break;

    case Operation::reduce:
      generateKeyValues_reduce(writer);
      break;

    default:
      LOG_SW(("Operation still not implemented"));
      break;
  }
}

void WorkerTask::generateTXT(TXTWriter *writer) {
  LOG_M(logs.background_process, ("Generate txt-data for task WT%lu", id()));

  // Type of inputs ( for selecting key-values )
  std::vector<KVFormat> inputFormats = operation_->getInputFormats();

  if (inputFormats.size() != 1) {
    setUserError("Parser with more that one input... that is not valid!");
    return;
  }

  // Run the generator over the ProcessWriter to emit all key-values
  ParserOut *parserOut = (ParserOut *)operation_->getInstance();

  if (!parserOut) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  KVInputVector inputVector(operation_);

  parserOut->environment = get_operation_environment();
  parserOut->tracer = this;
  parserOut->operationController = this;

  parserOut->init(writer);

  BlockList *list = block_list_container_.FindBlockList("input_0");

  if (list != NULL) {
    au::list<BlockRef>::iterator bi;
    for (bi = list->blocks_.begin(); bi != list->blocks_.end(); ++bi) {
      BlockRef *block_ref = *bi;
      BlockPointer block = block_ref->block();
      engine::BufferPointer buffer = block->buffer();

      // KV File to access content of this block
      au::SharedPointer<KVFile> file = block_ref->file();
      if (file == NULL) {
        LOG_E(logs.background_process, ("Error getting input_0. NULL txt file for block:%lu", block_ref->block_id()));
        setUserError("Error getting information about this block");
        return;
      }

      for (int hg = range_.hg_begin_; hg < range_.hg_end_; ++hg) {
        KVInfo info = file->info[hg];

        if (info.size > 0) {
          inputVector.prepareInput(info.kvs);
          inputVector.addKVs(0, info, file->kvs_for_hg(hg));

          KVSetStruct inputStruct;
          inputStruct.num_kvs = inputVector.num_kvs;
          inputStruct.kvs = inputVector._kv;

          parserOut->run(&inputStruct, writer);
        }
      }
    }
  }

  parserOut->finish(writer);

  // Detele the created instance
  delete parserOut;
}

void WorkerTask::generateKeyValues_map(samson::ProcessWriter *writer) {
  LOG_M(logs.background_process, ("generateKeyValues_map task WT%lu", id()));

  // Type of inputs ( for selecting key-values )
  std::vector<KVFormat> inputFormats = operation_->getInputFormats();

  if (inputFormats.size() != 1) {
    setUserError("Map with more that one input.");
    return;
  }

  // Input vector of key-values
  KVInputVector inputVector(operation_);

  // Get an instance of this operation
  Map *map = (Map *)operation_->getInstance();

  if (!map) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  // Prepare opertion to be executed
  map->environment = get_operation_environment();
  map->tracer = this;
  map->operationController = this;

  // Call the init function
  map->init(writer);

  BlockList *list = block_list_container_.FindBlockList("input_0");

  if (list != NULL) {
    au::list<BlockRef>::iterator bi;
    for (bi = list->blocks_.begin(); bi != list->blocks_.end(); ++bi) {
      BlockRef *block_ref = *bi;
      BlockPointer block = block_ref->block();
      engine::BufferPointer buffer = block->buffer();

      // Check header for valid block
      KVHeader *header = reinterpret_cast<KVHeader *>(buffer->data());
      if (!header->Check()) {
        setUserError(("Not valid header in block refernce"));
        return;
      }

      // Analyse all key-values and hashgroups
      au::SharedPointer<KVFile> file = block_ref->file();
      if (file == NULL) {
        LOG_E(logs.background_process, ("Error getting input_0. NULL KVfile for block:%lu", block_ref->block_id()));
        setUserError("Error getting information about this block");
        return;
      }

      // Read key values
      inputVector.prepareInput(header->info.kvs);

      for (int hg = range_.hg_begin_; hg < range_.hg_end_; ++hg) {
        inputVector.addKVs(0, file->info[hg], &file->kvs[file->kvs_index[hg]]);
      }

      // inputVector.addKVs(0, header->info, file->dataForHashGroup(range.hg_begin_));

      KVSetStruct inputStruct;
      inputStruct.num_kvs = inputVector.num_kvs;
      inputStruct.kvs = inputVector._kv;

      map->run(&inputStruct, writer);
    }
  }

  map->finish(writer);

  // Detele the created instance
  delete map;
}

#pragma mark

void WorkerTask::generateKeyValues_reduce(samson::ProcessWriter *writer) {
  LOG_M(logs.background_process, ("generateKeyValues_reduce task WT%lu", id()));

  au::Cronometer cronometer;
  LOG_M(logs.reduce_operation, ("[WT%lu:%s] Start reduce task", id(), au::str(cronometer.seconds(), "s").c_str()));

  bool update_only = stream_operation_->has_reduce_update_only() && stream_operation_->reduce_update_only();
  bool reduce_forward = stream_operation_->has_reduce_forward() && stream_operation_->reduce_forward();
  bool batch_operation = stream_operation_->has_batch_operation() && stream_operation_->batch_operation();

  size_t num_inputs = operation_->getNumInputs();
  // size_t num_outputs = operation_->getNumInputs();

  LOG_D(logs.reduce_operation, ("Reduce WT%lu - %s KVRange %s"
                                , worker_task_id()
                                , process_item_description().c_str()
                                , range_.str().c_str()));

  if (update_only) {
    LOG_D(logs.reduce_operation, ("Reduce WT%lu --> update_only", worker_task_id()));
  }

  if (reduce_forward) {
    LOG_D(logs.reduce_operation, ("Reduce WT%lu --> reduce_forward", worker_task_id()));
  }
  if (batch_operation) {
    LOG_D(logs.reduce_operation, ("Reduce WT%lu --> batch_operation", worker_task_id()));
  }

  // Get the operation instance
  Reduce *reduce = (Reduce *)operation_->getInstance();

  // Prepare the operation
  reduce->environment = get_operation_environment();
  reduce->tracer = this;
  reduce->operationController = this;

  // Init function
  reduce->init(writer);

  LOG_M(logs.reduce_operation, ("[WT%lu:%s] Collecting data...", id(), au::str(cronometer.seconds(), "s").c_str()));

  // Get the block reader list to prepare inputs for operation
  BlockReaderCollection blockreaderCollection(operation_);

  // Insert all the blocks involved in this operation
  for (int i = 0; i < operation_->getNumInputs(); ++i) {
    BlockList *list = block_list_container_.FindBlockList(au::str("input_%d", i));

    if (list != NULL) {
      au::list<BlockRef>::iterator bi;

      LOG_D(logs.reduce_operation, ("Reduce WT%lu - Adding Input %d : %lu blocks"
                                    , worker_task_id()
                                    , i
                                    , list->blocks_.size()));

      for (bi = list->blocks_.begin(); bi != list->blocks_.end(); ++bi) {
        BlockRef *block_ref = *bi;
        // BlockPointer block = block_ref->block();
        // engine::BufferPointer buffer = block->buffer();

        LOG_D(logs.reduce_operation, ("Reduce WT%lu - Adding block %lu (%s)"
                                      , worker_task_id()
                                      , block_ref->block_id()
                                      , block_ref->info().str().c_str()));

        // Process the last input as state ( if no batch operation, no reduce forward )
        if (!batch_operation && !reduce_forward && (i == (operation_->getNumInputs() - 1))) {
          blockreaderCollection.AddStateBlocks(block_ref, i);
        } else {
          blockreaderCollection.AddInputBlocks(block_ref, i);
        }
      }
    }
  }

  // Counters for statistics
  // int total_hgs = range_.hg_end_ - range_.hg_begin_;
  int processed_hds = 0;

  int transfered_states = 0;
  int processed_states = 0;

  LOG_D(logs.reduce_operation, ("Reduce WT%lu - %s Data ready! Running hash-groups"
                                , worker_task_id()
                                , process_item_description().c_str()));


  LOG_M(logs.reduce_operation, ("[WT%lu:%s] Processing data...", id(), au::str(cronometer.seconds(), "s").c_str()));

  for (int hg = 0; hg < KVFILE_NUM_HASHGROUPS; ++hg) {
    // Check if this is inside the range we are interested in processing
    if ((hg < range_.hg_begin_) || (hg >= range_.hg_end_)) {
      continue;
    }

    // Report progress to show in the lists
    ++processed_hds;
    // reportProgress((double)processed_hds / (double)total_hgs);

    // Prepare the blockReaderCollection for thi hash-group
    size_t num_kvs = blockreaderCollection.PrepareProcessingHashGroup(hg);

    if (num_kvs > 0) {
      LOG_D(logs.reduce_operation, ("Reduce WT%lu -  Process hg %d - %lu key-values"
                                    , worker_task_id()
                                    , hg
                                    , num_kvs));

      // Get data for the next reduce operation
      KVSetStruct *inputStructs = blockreaderCollection.GetNext();

      while (inputStructs) {
        // Trace with the number of key-values per channel
        if (num_inputs >= 2) {
          LOG_D(logs.reduce_operation, ("Reduce WT%lu - hg %d - Processing I:%lu S:%lu key-values"
                                        , worker_task_id()
                                        , hg
                                        , inputStructs[0].num_kvs
                                        , inputStructs[1].num_kvs
                                        ));
        }

        // Check if we have data in the input channels ( non state )
        bool input_data_available = false;
        for (size_t i = 0; i < (num_inputs - 1); ++i) {
          if (inputStructs[i].num_kvs > 0) {
            input_data_available = true;
          }
        }

        // Just copy state to the output if necessary
        if (update_only && !input_data_available) {
          if (inputStructs[1].num_kvs > 0) {  // Check we really have state to be copied
            int state_output = operation_->getNumOutputs() - 1;
            char *key_data = inputStructs[1].kvs[0]->key;  // Data pointer
            size_t key_size = inputStructs[1].kvs[0]->key_size;  // Jey size
            size_t value_size = inputStructs[1].kvs[0]->value_size;  // Value size
            writer->internal_emit(state_output, hg, key_data, key_size + value_size);
            ++transfered_states;
          } else {
            LOG_SW(("Interal error. No input and no state?"));
          }
        } else {
          // Call the reduce operation to generate output
          reduce->run(inputStructs, writer);
          ++processed_states;
        }

        // Get the next one
        inputStructs = blockreaderCollection.GetNext();
      }
    }
  }

  reduce->finish(writer);

  // Detele the created instance
  delete reduce;

  LOG_M(logs.reduce_operation, ("[WT%lu:%s] Finish reduce task", id(), au::str(cronometer.seconds(), "s").c_str()));
}

void WorkerTask::generateKeyValues_parser(samson::ProcessWriter *writer) {
  LOG_M(logs.background_process, ("generateKeyValues_parser task WT%lu", id()));

  // Run the generator over the ProcessWriter to emit all key-values
  Parser *parser = (Parser *)operation_->getInstance();

  if (!parser) {
    setUserError("Error getting an instance of this operation");
    return;
  }

  parser->environment = get_operation_environment();
  parser->tracer = this;
  parser->operationController = this;

  parser->init(writer);

  // Recover the input list "input_0"
  BlockList *list = block_list_container_.FindBlockList("input_0");

  if (list != NULL) {
    au::list<BlockRef>::iterator bi;
    for (bi = list->blocks_.begin(); bi != list->blocks_.end(); ++bi) {
      BlockRef *block_ref = *bi;
      BlockPointer block = block_ref->block();
      engine::BufferPointer buffer = block->buffer();

      // Pointer to the internal data in the buffer
      char *data = buffer->data() + sizeof(KVHeader);
      size_t size = buffer->size() - sizeof(KVHeader);

      // Run parser with this data
      parser->run(data, size, writer);
    }
  }

  parser->finish(writer);

  // Detele the created instance
  delete parser;
}

std::string WorkerTask::getStatus() {
  std::ostringstream output;

  output << "[" << worker_task_id() << "] ";
  output << "StreamOperation " << stream_operation_->name();
  output << " Operation " << stream_operation_->operation();
  return output.str();
}

std::string WorkerTask::str() {
  return au::str("Task WT%lu : %s %s %s", worker_task_id(), stream_operation_->name().c_str(),
                 stream_operation_->operation().c_str(), range_.str().c_str());
}

void WorkerTask::commit() {
  if (environment().Get("system.canceled_task", "no") == "yes") {
    LOG_M(logs.worker_task, ("Task %s not committed since it has been deactivated", str().c_str()));
    return;
  }

  // Process & commit outputs generated by this task
  // Here output blocks are created and added to blockmanager
  // DataModel is updated to keep them in block manager
  std::vector<size_t> new_block_ids = ProcessOutputBuffers();

  if (error_.HasErrors()) {
    std::string error_message = error_.GetLastError();
    LOG_M(logs.task_manager, ("Error in task %lu (%s)", id(), error_message.c_str()));
  } else {
    LOG_D(logs.task_manager, ("Task WT%lu blocks: %s", id(), str_block_ids().c_str()));
    LOG_M(logs.task_manager, ("Committing task W%lu (%s)", id(), str().c_str()));

    // Commit changes and release task
    std::string my_commit_command = commit_command();
    std::string caller = au::str("task WT%lu", worker_task_id());
    LOG_D(logs.task_manager, ("caller:'%s', commit_command:'%s'", caller.c_str(), my_commit_command.c_str()));
    samson_worker_->data_model()->Commit(caller, my_commit_command, error_);

    if (error_.HasErrors()) {
      LOG_W(logs.task_manager, ("Error committing task W%lu : %s"
                                , worker_task_id()
                                , error().GetLastError().c_str()));
    }
  }
}

FullKVInfo WorkerTask::GetStateDataInfo() const {
  if (state_input_channel_ == -1) {
    return FullKVInfo();
  }
  return GetInputInfo(state_input_channel_);
}

FullKVInfo WorkerTask::GetInputDataInfo() const {
  FullKVInfo info;

  for (int i = 0; i < num_input_channels_; ++i) {
    info.append(GetInputInfo(i));
  }
  return info;
}
}
}

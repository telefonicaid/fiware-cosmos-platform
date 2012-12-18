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
#include "samson/common/gpb_operations.h"

#include <algorithm>
#include <string>
#include <vector>
#include <set>

#include "au/log/LogMain.h"
#include "au/string/StringUtilities.h"
#include "au/string/Tokenizer.h"
#include "au/utils.h"
#include "engine/Engine.h"
#include "engine/MemoryManager.h"
#include "engine/ProcessManager.h"
#include "samson/stream/Block.h"



namespace samson {
namespace gpb {
gpb::StreamOperation *getStreamOperation(gpb::Data *data, const std::string& name) {
  for (int i = 0; i < data->operations_size(); ++i) {
    if (name == data->operations(i).name()) {
      return data->mutable_operations(i);
    }
  }
  return NULL;
}

gpb::StreamOperation *getStreamOperation(gpb::Data *data, size_t stream_operation_id) {
  for (int i = 0; i < data->operations_size(); ++i) {
    if (stream_operation_id == data->operations(i).stream_operation_id()) {
      return data->mutable_operations(i);
    }
  }
  return NULL;
}

void reset_stream_operations(gpb::Data *data) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::StreamOperation> *operations = data->mutable_operations();
  while (operations->size() > 0) {
    operations->RemoveLast();
  }

  ::google::protobuf::RepeatedPtrField< ::samson::gpb::QueueConnection> *connections = data->mutable_queue_connections();
  while (connections->size() > 0) {
    connections->RemoveLast();
  }

  ::google::protobuf::RepeatedPtrField< ::samson::gpb::BatchOperation> *batch_operations =
    data->mutable_batch_operations();
  while (batch_operations->size() > 0) {
    batch_operations->RemoveLast();
  }
}

void removeStreamOperation(gpb::Data *data, const std::string& name) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::StreamOperation> *operations = data->mutable_operations();

  for (int i = 0; i < operations->size(); ++i) {
    if (name == operations->Get(i).name()) {
      // Swap with the last element to remove
      operations->SwapElements(i, operations->size() - 1);
      operations->RemoveLast();
      return;
    }
  }
}

void setProperty(Environment *environment, const std::string& name, const std::string& value) {
  // Search for an existing variable
  for (int i = 0; i < environment->variable_size(); ++i) {
    if (environment->variable(i).name() == name) {
      environment->mutable_variable(i)->set_value(value);
      return;
    }
  }

  // Create a new entry
  EnvironmentVariable *ev = environment->add_variable();
  ev->set_name(name);
  ev->set_value(value);
}

std::string getProperty(Environment *environment, const std::string& name, const std::string& default_value) {
  // Search for an existing variable
  for (int i = 0; i < environment->variable_size(); ++i) {
    if (environment->variable(i).name() == name) {
      return environment->variable(i).value();
    }
  }
  return default_value;
}

void unsetProperty(Environment *environment, const std::string& name) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::EnvironmentVariable> *e = environment->mutable_variable();

  for (int i = 0; i < e->size(); ++i) {
    if (e->Get(i).name() == name) {
      e->SwapElements(i, e->size() - 1);
      e->RemoveLast();
      return;
    }
  }
}

void setPropertyInt(Environment *environment, const std::string& name, int value) {
  setProperty(environment, name, au::str("%d", value));
}

int getPropertyInt(Environment *environment, const std::string& name, int default_value) {
  std::string v = getProperty(environment, name, "no-value");

  if (v == "no-value") {
    return default_value;
  } else {
    return atoi(v.c_str());
  }
}

std::string str(const Environment& environment) {
  std::ostringstream output;

  for (int i = 0; i < environment.variable_size(); ++i) {
    output << environment.variable(i).name() << "=" << environment.variable(i).value() << " ";
  }
  return output.str();
}

bool isWorkerIncluded(ClusterInfo *cluster_information, size_t worker_id) {
  for (int i = 0; i < cluster_information->workers_size(); ++i) {
    if (cluster_information->workers(i).worker_id() == worker_id) {
      return true;
    }
  }
  return false;
}

size_t GetNumKVRanges(ClusterInfo *cluster_information, size_t worker_id) {
  size_t total = 0;

  for (int i = 0; i < cluster_information->process_units_size(); ++i) {
    if (cluster_information->process_units(i).worker_id() == worker_id) {
      total++;
    }
  }
  return total;
}

void reset_data(Data *data) {
  // Remove all queues except .modules
  ::samson::gpb::Queue *modules_queue = NULL;
  ::samson::gpb::Queue *current_modules_queue = get_queue(data, ".modules");
  if (current_modules_queue) {
    modules_queue = new ::samson::gpb::Queue();
    modules_queue->CopyFrom(*current_modules_queue);
  }

  ::google::protobuf::RepeatedPtrField< ::samson::gpb::Queue> *queues = data->mutable_queue();
  while (queues->size() > 0) {
    queues->RemoveLast();
  }

  if (modules_queue) {
    queues->Add()->CopyFrom(*modules_queue);
    delete modules_queue;
  }
}

Queue *get_or_create_queue(Data *data, const std::string& queue_name, KVFormat format, au::ErrorManager& error) {
  Queue *queue = get_queue(data, queue_name, format, error);

  if (error.HasErrors()) {
    return NULL;
  }

  if (queue) {
    return queue;
  }

  // Create a new one with the rigth format
  queue = data->add_queue();
  queue->set_name(queue_name);
  queue->set_commit_id(data->commit_id());
  queue->set_key_format(format.keyFormat);
  queue->set_value_format(format.valueFormat);
  queue->mutable_blocks();   // Just to force the creation of this element
  return queue;
}

Queue *get_queue(Data *data, const std::string& queue_name, KVFormat format, au::ErrorManager& error) {
  for (int i = 0; i < data->queue_size(); ++i) {
    if (data->queue(i).name() == queue_name) {
      // Check formatblo
      Queue *queue = data->mutable_queue(i);

      if (queue->key_format() != format.keyFormat) {
        error.AddError(
          au::str("Wrong key-format for queue %s (%s vs %s)", queue_name.c_str(), queue->key_format().c_str(),
                  format.keyFormat.c_str()));
        return NULL;
      }

      if (queue->value_format() != format.valueFormat) {
        error.AddError(
          au::str("Wrong value-format for queue %s (%s vs %s)", queue_name.c_str(),
                  queue->value_format().c_str(), format.valueFormat.c_str()));
        return NULL;
      }

      return queue;
    }
  }

  return NULL;
}

Queue *get_queue(Data *data, const std::string& queue_name) {
  for (int i = 0; i < data->queue_size(); ++i) {
    if (data->queue(i).name() == queue_name) {
      return data->mutable_queue(i);
    }
  }
  return NULL;
}

void removeQueue(Data *data, const std::string& name) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::Queue> *queues = data->mutable_queue();

  for (int i = 0; i < queues->size(); ++i) {
    if (queues->Get(i).name() == name) {
      queues->SwapElements(i, queues->size() - 1);
      queues->RemoveLast();
      return;
    }
  }
}

void getQueueInfo(const gpb::Queue& queue, size_t *num_blocks, size_t *kvs, size_t *size) {
  *kvs = 0;
  *size = 0;
  *num_blocks = queue.blocks_size();
  for (int i = 0; i < queue.blocks_size(); ++i) {
    *kvs += queue.blocks(i).kvs();
    *size += queue.blocks(i).size();
  }
}

::samson::FullKVInfo getKVInfoForQueue(const gpb::Queue& queue) {
  ::samson::FullKVInfo info;
  for (int i = 0; i < queue.blocks_size(); ++i) {
    info.append(::samson::FullKVInfo(queue.blocks(i).size(), queue.blocks(i).kvs()));
  }
  return info;
}

::samson::BlockKVInfo getBlockKVInfoForQueue(const gpb::Queue& queue, ::samson::KVRange range) {
  ::samson::BlockKVInfo info;
  for (int i = 0; i < queue.blocks_size(); ++i) {
    double factor = range.GetOverlapFactor(queue.blocks(i).range());
    info.AppendBlock(::samson::FullKVInfo(factor * queue.blocks(i).size(), factor * queue.blocks(i).kvs()));
  }
  return info;
}

void add_block(Data *data, const std::string& queue_name, size_t block_id, size_t block_size, KVFormat format,
               ::samson::KVRange range, ::samson::KVInfo info, int version, au::ErrorManager& error) {
  // Get or create this queue
  gpb::Queue *queue = get_or_create_queue(data, queue_name, format, error);

  if ((queue == NULL) || error.HasErrors()) {
    return;
  }

  queue->set_commit_id(data->commit_id());

  // Add a new block reference
  Block *block = queue->add_blocks();
  block->set_block_id(block_id);
  block->set_block_size(block_size);
  block->set_kvs(info.kvs);
  block->set_size(info.size);
  block->set_time(time(NULL));
  block->set_commit_id(version);

  // Add range to this block
  KVRange *gpb_range = block->mutable_range();
  gpb_range->set_hg_begin(range.hg_begin_);
  gpb_range->set_hg_end(range.hg_end_);
}

void rm_block(Data *data, const std::string& queue_name, size_t block_id, KVFormat format, ::samson::KVRange range,
              ::samson::KVInfo info, int version, au::ErrorManager& error) {
  // Get the queue
  gpb::Queue *queue = get_queue(data, queue_name, format, error);

  if (error.HasErrors()) {
    return;
  }

  if (!queue) {
    error.AddError(au::str("Queue %s does not exist", queue_name.c_str()));
    return;
  }

  queue->set_commit_id(data->commit_id());

  // Remove the first time ( and probably the only one ) this block is in the queue
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::Block> *blocks = queue->mutable_blocks();
  for (int i = 0; i < blocks->size(); ++i) {
    if (blocks->Get(i).block_id() == block_id) {
      // Check information is correct
      if (blocks->Get(i).size() != info.size) {
        error.AddError(au::str("Error removing block %s in queue %s ( size mismatch %lu != %lu"
                               , str_block_id(block_id).c_str()
                               , queue_name.c_str()
                               , blocks->Get(i).size()
                               , info.size));
      }
      if (blocks->Get(i).kvs() != info.kvs) {
        error.AddError(au::str("Error removing block %s in queue %s ( #kvs mismatch %lu != %lu"
                               , str_block_id(block_id).c_str()
                               , queue_name.c_str()
                               , blocks->Get(i).kvs()
                               , info.kvs));
      }

      // The only options is moving this element until the end of the vector and remove it!
      for (int j = i; j < (blocks->size() - 1); ++j) {
        blocks->SwapElements(j, j + 1);
      }
      blocks->RemoveLast();
      return;
    }
  }
}

size_t range_overlap(int hg_begin1, int hg_end1, int hg_begin2, int hg_end2) {
  int hg_begin = std::max(hg_begin1, hg_begin2);
  int hg_end = std::min(hg_end1, hg_end2);

  if (hg_begin < hg_end) {
    return (hg_end - hg_begin);
  } else {
    return 0;
  }
}

bool data_exist_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& queue_target) {
  for (int i = 0; i < data->queue_connections_size(); ++i) {
    if (data->queue_connections(i).queue_source() == queue_source) {
      if (data->queue_connections(i).queue_target() == queue_target) {
        return true;
      }
    }
  }

  return false;
}

void data_create_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& queue_target) {
  gpb::QueueConnection *queue_connection = data->add_queue_connections();

  queue_connection->set_queue_source(queue_source);
  queue_connection->set_queue_target(queue_target);
}

void data_remove_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& target_source) {
  ::google::protobuf::RepeatedPtrField<gpb::QueueConnection> *queue_connections = data->mutable_queue_connections();

  for (int i = 0; i < queue_connections->size(); ++i) {
    if (queue_connections->Get(i).queue_source() == queue_source) {
      if (queue_connections->Get(i).queue_target() == target_source) {
        // Remove this element
        queue_connections->SwapElements(i, queue_connections->size() - 1);
        queue_connections->RemoveLast();
        return;
      }
    }
  }
}

au::StringVector data_get_queues_connected(gpb::Data *data, const std::string& queue_source) {
  au::StringVector target_queues;

  for (int i = 0; i < data->queue_connections_size(); ++i) {
    if (data->queue_connections(i).queue_source() == queue_source) {
      target_queues.push_back(data->queue_connections(i).queue_target());
    }
  }
  return target_queues;
}

bool batch_operation_is_finished(gpb::Data *data, const gpb::BatchOperation& batch_operation) {
  size_t delilah_id = batch_operation.delilah_id();
  size_t delilah_component_id = batch_operation.delilah_component_id();
  std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);

  // @andreu: We should check all inputs since it is a batch operation
  for (int i = 0; i < batch_operation.inputs_size(); ++i) {
    std::string queue_name = prefix + batch_operation.inputs(i);
    Queue *queue = get_queue(data, queue_name);
    if ((queue) && (getKVInfoForQueue(*queue).size > 0)) {
      return false;
    }
  }

  return true;
}

bool string_starts_with(const std::string& s, const std::string& prefix) {
  if (s.length() < prefix.length()) {
    return false;
  }

  return (s.substr(0, prefix.length()) == prefix);
}

void remove_finished_operation(gpb::Data *data, bool all_flag) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::BatchOperation> *operations = data->mutable_batch_operations();

  int i = 0;
  while (i < operations->size()) {
    if (!all_flag) {
      if (!operations->Get(i).finished()) {
        i++;
        continue;
      }
    }

    // Remove associated stream operation and queues
    size_t delilah_id = operations->Get(i).delilah_id();
    size_t delilah_component_id = operations->Get(i).delilah_component_id();

    std::string prefix = au::str(".%s_%lu_", au::code64_str(delilah_id).c_str(), delilah_component_id);

    // Remove all queues and stream operations starting with this....
    ::google::protobuf::RepeatedPtrField< ::samson::gpb::Queue> *queues = data->mutable_queue();
    for (int j = 0; j < queues->size(); ++j) {
      std::string name = queues->Get(j).name();
      if (string_starts_with(name, prefix)) {
        // Remove queue
        queues->SwapElements(j, queues->size() - 1);
        queues->RemoveLast();
      }
    }

    // Remove all queues and stream operations starting with this....
    ::google::protobuf::RepeatedPtrField< ::samson::gpb::StreamOperation> *stream_operations =
      data->mutable_operations();
    for (int j = 0; j < stream_operations->size(); ++j) {
      std::string name = stream_operations->Get(j).name();
      if (string_starts_with(name, prefix)) {
        // Remove queue
        stream_operations->SwapElements(j, stream_operations->size() - 1);
        stream_operations->RemoveLast();
      }
    }

    // Remove this element
    operations->SwapElements(i, operations->size() - 1);
    operations->RemoveLast();
  }
}

void AddBlockIds(const gpb::Queue&  queue, const std::vector<samson::KVRange>&ranges, std::set<size_t>& block_ids) {
  for (int b = 0; b < queue.blocks_size(); ++b) {
    const gpb::Block& block = queue.blocks(b);
    samson::KVRange range = block.range();     // Implicit conversion
    if (range.IsOverlapped(ranges)) {
      block_ids.insert(block.block_id());
    }
  }
}

void AddStateBlockIds(gpb::Data *data, const std::vector<samson::KVRange>&ranges, std::set<size_t>& block_ids) {
  // Search all operations to detect states
  for (int i = 0; i < data->operations_size(); ++i) {
    if (data->operations(i).batch_operation()) {
      continue;
    }
    if (data->operations(i).reduce_forward()) {
      continue;
    }
    if (data->operations(i).inputs_size() < 2) {
      continue;
    }

    // State is the last one
    std::string queue_name = data->operations(data->operations_size() - 1).name();
    gpb::Queue *queue = get_queue(data, queue_name);
    if (queue) {
      AddBlockIds(*queue, ranges, block_ids);
    }
  }
}

void AddBlockIds(gpb::Data *data, const std::vector<samson::KVRange>&ranges, std::set<size_t>& block_ids) {
  // Loop all the queues
  for (int q = 0; q < data->queue_size(); ++q) {
    const gpb::Queue& queue = data->queue(q);
    AddBlockIds(queue, ranges, block_ids);
  }
}

void AddBlockIds(gpb::Data *data, std::set<size_t>& block_ids) {
  // add all blocks from all queues
  for (int q = 0; q < data->queue_size(); ++q) {
    const gpb::Queue& queue = data->queue(q);
    for (int b = 0; b < queue.blocks_size(); ++b) {
      block_ids.insert(queue.blocks(b).block_id());
    }
  }
}

FullKVInfo GetFullKVInfo(gpb::Data *data) {
  FullKVInfo info;

  for (int q = 0; q < data->queue_size(); ++q) {
    for (int b = 0; b < data->queue(q).blocks_size(); ++b) {
      size_t size = data->queue(q).blocks(b).size();
      size_t kvs = data->queue(q).blocks(b).kvs();
      info.append(size, kvs);
    }
  }
  return info;
}

DataInfoForRanges get_data_info_for_ranges(gpb::Data *data
                                           , const std::string& queue
                                           , const std::vector<samson::KVRange>& ranges) {
  std::vector<std::string> queues;
  queues.push_back(queue);
  return get_data_info_for_ranges(data, queues, ranges);
}

DataInfoForRanges get_data_info_for_ranges(gpb::Data *data
                                           , const std::vector<std::string>& queues
                                           , const std::vector<samson::KVRange>& ranges) {
  DataInfoForRanges info;

  for (size_t q = 0; q < queues.size(); ++q) {
    gpb::Queue *queue = gpb::get_queue(data, queues[q]);
    if (!queue) {
      continue;
    }

    for (int b = 0; b < queue->blocks_size(); ++b) {
      const gpb::Block& block = queue->blocks(b);
      info.data_size += block.size();
    }

    std::vector<double> defrag_factors;

    for (size_t r = 0; r < ranges.size(); ++r) {
      size_t range_memory_size = 0;
      size_t range_data_size = 0;

      for (int b = 0; b < queue->blocks_size(); ++b) {
        const gpb::Block& block = queue->blocks(b);
        size_t memory_size = block.block_size();
        samson::KVRange block_range = block.range();   // Implicit conversion

        double overlap_factor = block_range.GetOverlapFactor(ranges[r]);
        size_t data_size = overlap_factor * block.size();
        size_t kvs_size = overlap_factor * block.kvs();

        if (overlap_factor > 0) {
          info.data_size_in_ranges += data_size;   // Total size of data in this queue for selected ranges
          info.data_kvs_in_ranges += kvs_size;   // Total size of data in this queue for selected ranges

          range_memory_size += memory_size;
          range_data_size += data_size;
        }
      }

      // Compute defrag factor
      if (range_data_size > 0) {
        defrag_factors.push_back(static_cast<double>(range_data_size) / static_cast<double>(range_memory_size));
      }

      // Collect maximum
      if (range_memory_size > info.max_memory_size_for_a_range) {
        info.max_memory_size_for_a_range = range_memory_size;
      }
      if (range_data_size > info.max_data_size_for_a_range) {
        info.max_data_size_for_a_range = range_data_size;
      }
    }

    // Compute average defrag_fractor
    double total_defrag_factor = 0;
    for (size_t i = 0; i < defrag_factors.size(); ++i) {
      total_defrag_factor += defrag_factors[i];
    }

    info.defrag_factor = total_defrag_factor / static_cast<double>(defrag_factors.size());
  }
  return info;
}

void limit_last_commits(gpb::Data *data) {
  if (data->last_commits_size() < 20) {
    return;
  }

  std::vector<std::string> messages;
  int size = data->last_commits_size();
  for (int i = size - 20; i < size; ++i) {
    messages.push_back(data->last_commits(i));
  }

  data->clear_last_commits();
  for (size_t i = 0; i < messages.size(); ++i) {
    data->add_last_commits(messages[i]);
  }
}

std::string Get(const gpb::CollectionRecord& record, const std::string& field) {
  for (int i = 0; i < record.item_size(); ++i) {
    if (record.item(i).name() == field) {
      return record.item(i).value();
    }
  }
  return "";
}

void Sort(gpb::Collection *collection, const std::string& field) {
  ::google::protobuf::RepeatedPtrField< ::samson::gpb::CollectionRecord> *records = collection->mutable_record();

  for (int i = 0; i < records->size(); ++i) {
    std::string f1 = Get(records->Get(i), field);
    for (int j = i + 1; j < records->size(); ++j) {
      std::string f2 = Get(records->Get(j), field);
      if (f1 > f2) {
        records->SwapElements(i, j);
      }
    }
  }
}

void UpdateEnvironment(gpb::Environment *environment, const std::string& env, au::ErrorManager &error) {
  au::token::Tokenizer tokenizer(",=");
  au::token::TokenVector token_vector = tokenizer.Parse(env);

  while (!token_vector.eof()) {
    au::token::Token *token = token_vector.PopToken();
    std::string concept = token->content();

    if (!token_vector.CheckNextTokenContentIs("=")) {
      token_vector.set_error(error, au::str("'=' not found when processing '%s'", env.c_str()));
      return;
    }

    // Remove "="
    token_vector.PopToken();

    token = token_vector.PopToken();
    if (!token) {
      token_vector.set_error(error,
                             au::str("No value for property '%s' when processing '%s'", concept.c_str(), env.c_str()));
      return;
    }
    std::string value = token->content();

    if (!token_vector.eof()) {
      if (!token_vector.CheckNextTokenContentIs(",")) {
        token_vector.set_error(error, au::str("',' not found when processing '%s'", env.c_str()));
        return;
      }

      // Remove ","
      token_vector.PopToken();
    }

    // Set the value in the provided environment variable
    setProperty(environment, concept, value);
  }
}

void RemoveModules(gpb::Data *data, const std::string pattern) {
  // Remove blocks in queue .modules
  gpb::Queue *queue = gpb::get_queue(data, ".modules");

  if (!queue) {
    return;    // Nothing to remove
  }

  ::google::protobuf::RepeatedPtrField< ::samson::gpb::Block > *blocks = queue->mutable_blocks();
  au::SimplePattern simple_pattern(pattern);
  bool update_queue_commit_id = false;
  for (int i = 0; i < blocks->size(); ) {
    size_t block_id = blocks->Get(i).block_id();
    std::string block_name = str_block_id(block_id);
    if (simple_pattern.match(block_name)) {
      // Remove block
      update_queue_commit_id = true;
      blocks->SwapElements(i, blocks->size() - 1);
      blocks->RemoveLast();
    } else {
      ++i;
    }
  }

  if (update_queue_commit_id) {
    queue->set_commit_id(data->commit_id());
  }
}
}
}

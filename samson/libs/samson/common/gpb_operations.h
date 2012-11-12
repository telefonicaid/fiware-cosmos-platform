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

#ifndef _H_SAMOSN_GPB_OPERATIONS
#define _H_SAMOSN_GPB_OPERATIONS

#include <string>
#include <vector>

#include "au/ErrorManager.h"
#include "au/containers/SharedPointer.h"
#include "au/containers/StringVector.h"
#include "samson/common/BlockKVInfo.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/common/KVRange.h"
#include "samson/module/KVFormat.h"

#include "samson/common/samson.pb.h"

namespace samson {
namespace gpb {
typedef au::SharedPointer<Collection>   CollectionPointer;

// Get all blocks in a datamodel
void AddBlockIds(gpb::Data *data, std::set<size_t>& block_ids);
void AddBlockIds(gpb::Data *data, const std::vector<samson::KVRange>&ranges, std::set<size_t>& block_ids);
FullKVInfo GetFullKVInfo(gpb::Data *data);

// Operations over gpb::StreamOperations
gpb::StreamOperation *getStreamOperation(gpb::Data *data, const std::string& name);
gpb::StreamOperation *getStreamOperation(gpb::Data *data, size_t stream_operation_id);
void removeStreamOperation(gpb::Data *data, const std::string& name);
void reset_stream_operations(gpb::Data *data);

// Operation over gpb::Environemnt
std::string str(const Environment& environment);
void setProperty(Environment *environment, const std::string& property, const std::string& value);
std::string getProperty(Environment *environment, const std::string& property, const std::string& default_value);
void unsetProperty(Environment *environment, const std::string& property);
void setPropertyInt(Environment *environment, const std::string& property, int value);
int getPropertyInt(Environment *environment, const std::string& property, int default_value);

// Operation over gpb::ClusterInfo
bool isWorkerIncluded(ClusterInfo *cluster_information, size_t worker_id);
size_t GetNumKVRanges(ClusterInfo *cluster_information, size_t worker_id);

// Operations over DataModel
void reset_data(Data *data);
Queue *get_queue(Data *data, const std::string& queue_name);
Queue *get_queue(Data *data, const std::string& queue_name, KVFormat format, au::ErrorManager&error);
Queue *get_or_create_queue(Data *data, const std::string& name, KVFormat format, au::ErrorManager&error);
void removeQueue(Data *data, const std::string& name);

// Operations over Queue
void getQueueInfo(const gpb::Queue& queue, size_t *num_blocks, size_t *kvs, size_t *size);
::samson::FullKVInfo getKVInfoForQueue(const gpb::Queue& queue);
::samson::BlockKVInfo getBlockKVInfoForQueue(const gpb::Queue& queue, ::samson::KVRange range);

// Add and Remove ranges in block references
void add_block(Data *data, const std::string& queue_name, size_t block_id, size_t block_size, KVFormat format,
               ::samson::KVRange range, ::samson::KVInfo info, int version, au::ErrorManager&error);

void rm_block(Data *data, const std::string& queue_name, size_t block_id, KVFormat format, ::samson::KVRange range,
              ::samson::KVInfo info, int version, au::ErrorManager&error);


// Queue connections
bool data_exist_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& queue_target);
void data_create_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& queue_target);
au::StringVector data_get_queues_connected(gpb::Data *data, const std::string& queue_source);
void data_remove_queue_connection(gpb::Data *data, const std::string& queue_source, const std::string& target_source);

// Batch operation
bool batch_operation_is_finished(gpb::Data *data, const gpb::BatchOperation& batch_operation);
void remove_finished_operation(gpb::Data *data, bool all_flag);

// Compute information for a queue in a set of ranges
class DataInfoForRanges {
public:
  DataInfoForRanges() {
    data_size = 0;
    data_kvs_in_ranges = 0;
    data_size_in_ranges = 0;
    max_memory_size_for_a_range = 0;
    max_data_size_for_a_range = 0;
    defrag_factor = 0;
  }

  size_t data_size;
  size_t data_kvs_in_ranges;
  size_t data_size_in_ranges;
  size_t max_memory_size_for_a_range;
  size_t max_data_size_for_a_range;
  double defrag_factor;
};

DataInfoForRanges get_data_info_for_ranges(gpb::Data *data
                                           , const std::vector<std::string>& queues
                                           , const std::vector<samson::KVRange>& ranges);
DataInfoForRanges get_data_info_for_ranges(gpb::Data *data
                                           , const std::string& queue
                                           , const std::vector<samson::KVRange>& ranges);
}
}   // End of namespace samson::gpb

#endif  // ifndef _H_SAMOSN_GPB_OPERATIONS

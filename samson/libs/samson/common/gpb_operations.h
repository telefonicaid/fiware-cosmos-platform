


#ifndef _H_SAMOSN_GPB_OPERATIONS
#define _H_SAMOSN_GPB_OPERATIONS

#include "au/ErrorManager.h"
#include "au/containers/StringVector.h"
#include "samson/common/FullKVInfo.h"
#include "samson/common/KVInfo.h"
#include "samson/common/KVRange.h"
#include "samson/module/KVFormat.h"

#include "samson.pb.h"

namespace samson {
namespace gpb {
// Operations over gpb::StreamOperations
gpb::StreamOperation *getStreamOperation(gpb::Data *data, const std::string& name);
void removeStreamOperation(gpb::Data *data, const std::string& name);
void reset_stream_operations(gpb::Data *data);

// Operation over gpb::Environemnt
std::string str(const Environment& environment);
void setProperty(Environment *environment, const std::string& property, const std::string& value);
std::string getProperty(Environment *environment, const std::string& property,
                        const std::string& default_value);
void unsetProperty(Environment *environment, const std::string& property);
void setPropertyInt(Environment *environment, const std::string& property, int value);
int getPropertyInt(Environment *environment, const std::string& property, int default_value);

// Operation over gpb::ClusterInfo
bool isWorkerIncluded(ClusterInfo *cluster_information, size_t worker_id);

// Operations over DataModel
void reset_data(Data *data);
Queue *get_queue(Data *data, const std::string& queue_name);
Queue *get_queue(Data *data, const std::string& queue_name,  KVFormat format,
                 au::ErrorManager *error);
Queue *get_or_create_queue(Data *data, const std::string& name, KVFormat format,
                           au::ErrorManager *error);

void removeQueue(Data *data, const std::string& name);

// Operations over Queue
void getQueueInfo(const gpb::Queue& queue, size_t *num_blocks, size_t *kvs, size_t *size);
::samson::FullKVInfo getKVInfoForQueue(const gpb::Queue& queue);

// Add and Remove ranges in block references
void add_block(Data *data
               , const std::string& queue_name
               , size_t block_id
               , KVFormat format
               , ::samson::KVRange range
               , ::samson::KVInfo info
               , int version
               , au::ErrorManager *error);

void rm_block(Data *data
              , const std::string& queue_name
              , size_t block_id
              , KVFormat format
              , ::samson::KVRange range
              , ::samson::KVInfo info
              , au::ErrorManager *error);


Block *get_first_block(Queue *queue, size_t block_id);
void erase_block(Queue *queue, Block *block);

// Queue connections
bool data_exist_queue_connection(gpb::Data *data, const std::string& queue_source,
                                 const std::string& queue_target);
void data_create_queue_connection(gpb::Data *data, const std::string& queue_source,
                                  const std::string& queue_target);
au::StringVector data_get_queues_connected(gpb::Data *data, const std::string& queue_source);
void data_remove_queue_connection(gpb::Data *data, const std::string& queue_source,
                                  const std::string& target_source);

// Batch operation
bool bath_operation_is_finished(gpb::Data *data, const gpb::BatchOperation& batch_operation);
}
}  // End of namespace samson::gpb

#endif  // ifndef _H_SAMOSN_GPB_OPERATIONS
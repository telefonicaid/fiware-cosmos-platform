#ifndef _H_SAMSON_PUSH_OPERATION
#define _H_SAMSON_PUSH_OPERATION


#include "engine/Notification.h"
#include "engine/NotificationListener.h"
#include "samson/common/KVHeader.h"
#include "samson/common/Visualitzation.h"
#include <string>

namespace samson {
class NetworkInterface;
class Info;
class Packet;
class SamsonWorker;
}

namespace samson {
namespace worker {
//
// Push operation: operation to add a buffer to a queue
//

class PushOperation : public engine::NotificationListener {
public:

  PushOperation(SamsonWorker *samson_worker
                , size_t block_id
                , size_t delilah_id
                , size_t push_id
                , engine::BufferPointer buffer
                , const std::vector<std::string>& queues);

  // engine::NotificationListener
  virtual void notify(engine::Notification *notification);

  // Commit message from delilah
  void commit();

  // Get information about this push operation ( worker side )
  std::string getStrIdentifiers();
  size_t get_block_id();
  size_t get_delilah_id();
  size_t get_push_id();
  std::string getStrBufferInfo();
  std::string getStatus();

  // Create packet to be sent to delilah client
  Packet *response_packet(bool commit);

private:

  ::samson::SamsonWorker * samson_worker_;  // Pointer to samson worker

  size_t block_id_;                        // Identifier of the block created ( given by BLockManager )

  size_t delilah_id_;                      // Identifier of deliah node that should be notfied when finish
  size_t push_id_;                         // Identifier of the concrete push in this delilah

  // Buffer information ( for table visualization )
  KVHeader header;

  // Vector of queues to add this block
  std::vector<std::string> queues_;

  bool distributed;                  // Flag to indicate that this block has been correctly distributed
  bool commited;                     // Falg to indicate that this block gas been correctly commited
};

// Manager of the push operations at worker side
class PushManager {
public:

  PushManager(SamsonWorker *samson_worker) {
    samson_worker_ = samson_worker;
  }

  // Reveide a message from a delilah
  void receive_push_block(size_t delilah_id
                          , size_t push_id
                          , engine::BufferPointer buffer
                          , const std::vector<std::string>& queues);

  void receive_push_block_commit(size_t delilah_id, size_t push_id);

  // Get a collection with a single record with information for this worker...
  gpb::Collection *getCollectionForPushOperations(const Visualization& visualization);

private:

  // Pointer to SamsonWorker to interact with the rest of elements
  SamsonWorker *samson_worker_;

  // Vector of current operations
  au::vector<PushOperation> push_operations_;
};
}
}                       // End of namespae samson

#endif  // ifndef _H_SAMSON_PUSH_OPERATION

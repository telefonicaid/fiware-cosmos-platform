

#ifndef _H_SAMSON_PUSH_MANAGER
#define _H_SAMSON_PUSH_MANAGER

#include "au/containers/map.h"
#include "au/tables/Table.h"


namespace samson {
class Delilah;
class Packet;

class PushItem {
  typedef enum {
    init,
    waiting_push_confirmation,
    ready_for_commit,
    waiting_commig_confirmation,
    completed
  } PushItemState;

  size_t push_id_;                  // Identifier of this push
  engine::BufferPointer buffer_;                // Buffer considered in this push

  // Queues to send data
  std::vector<std::string> queues_;

  // Information about SAMSON push try
  size_t worker_id_;                // Worker_id used for this push
  au::Cronometer cronometer_;       // Time since I send packet

  // Internal state
  PushItemState state;

  // Pointer to delilah client to send packets and check for workers
  Delilah *delilah_;

  friend class PushManager;

public:

  PushItem(Delilah *delilah, size_t item_id, engine::BufferPointer buffer, const std::vector<std::string>& queues);

  bool isFinished();
  bool isReadyForCommit();

  void review();

  // Reset push item
  void reset();

  // Messages recevied form workers
  void receive(Message::MessageCode mdgCode, size_t worker_id);

  // Send commit message ( only in ready_for_commit state )
  void send_commit();

  // Get identifier
  size_t get_push_id() {
    return push_id_;
  }

  // Get a description of current status
  std::string str() {
    switch (state) {
      case init: return "Uninitialized";

      case waiting_push_confirmation: return au::str("Waiting distribution confirmation from worker %lu", worker_id_);

      case ready_for_commit: return "Ready for commit";

      case waiting_commig_confirmation: return au::str("Waiting commit confirmation from worker %lu", worker_id_);

      case completed: return "Finalized";
    }

    return "Error";
  }

  size_t get_time() {
    return cronometer_.seconds();
  }

  std::string str_buffer_info() {
    if (buffer_ != NULL) {
      return au::str(buffer_->getSize(), "B");
    } else {
      return "No buffer";
    }
  }

  size_t size() {
    if (buffer_ != NULL) {
      return buffer_->getSize();
    } else {
      return 0;
    }
  }
};


class PushManager {
  au::map<size_t, PushItem> items_;    // Items currently uploading to the cluster
  size_t item_id_;                     // Next identifier for a push item
  Delilah *delilah_;                   // Pointer to delilah to send and receive packets

public:

  PushManager(Delilah *delilah);

  // Main function to push data to SAMSON
  size_t push(engine::BufferPointer buffer, const std::vector<std::string>& queues);

  // Receive this packet from worker
  void receive(Message::MessageCode mdgCode, size_t worker_id, size_t push_id);

  // Reset a particular push_id element
  void reset(size_t push_id);

  // General review function
  void review();

  // Get table of collections
  au::tables::Table *getTableOfItems();

  // Get the number of pushing elments
  size_t get_num_items();
};
}  // end of namespace samson

#endif // ifndef _H_SAMSON_PUSH_MANAGER

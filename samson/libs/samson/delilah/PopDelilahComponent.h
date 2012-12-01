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
#ifndef _H_POP_DELILAH_COMPONENT
#define _H_POP_DELILAH_COMPONENT

#include <fstream>  // ifstream
#include <iostream>  // std::cout
#include <list>  // std::list
#include <set>  // std::vector
#include <sstream>  // std::ostringstream
#include <vector>  // std::vector


#include "au/ErrorManager.h"  // au::ErrorManager
#include "au/log/LogMain.h"
#include "au/statistics/CounterCollection.h"   // au::CounterCollection
#include "au/statistics/Cronometer.h"      // au::Cronometer
#include "au/string/Descriptors.h"       // au::CounterCollection
#include "au/string/StringUtilities.h"  // au::Format

#include "engine/DiskManager.h"     // engine::DiskManager
#include "engine/Engine.h"               // engine::NotificationListener
#include "engine/NotificationListener.h"  // engine::NotificationListener

#include "samson/common/Logs.h"
// samson::network

#include "DelilahComponent.h"  // samson::DelilahComponent
#include "samson/delilah/Delilah.h"  // samson::Delilah

#include "DataSource.h"  // samson::TXTFileSet


namespace samson {
class Delilah;
class Buffer;


class PopDelilahComponentItem {
public:

  PopDelilahComponentItem(size_t pop_id, size_t block_id, size_t commit_id) {
    pop_id_ = pop_id;
    block_id_ = block_id;
    commit_id_ = commit_id;
  }

  void SetContent(engine::BufferPointer buffer) {
    if (buffer_ != NULL) {
      LOG_SW(("Ignoring data received at PopDelilahComponentItem. Already received"));
      return;
    }
    buffer_ = buffer;
  }

  size_t worker_id() {
    return worker_id_;
  }

  void set_worker_id(size_t worker_id) {
    worker_id_ = worker_id;
  }

  engine::BufferPointer buffer() {
    return buffer_;
  }

  size_t block_id() {
    return block_id_;
  }

  const au::Cronometer& cronometer() {
    return cronometer_;
  }

  bool worker_confirmation() {
    return worker_confirmation_;
  }

  void SetWorkerConfirmation() {
    worker_confirmation_ = true;
  }

  void ResetRequest() {
    cronometer_.Reset();
    worker_confirmation_ = false;
  }

  size_t pop_id() {
    return pop_id_;
  }

  size_t commit_id() const {
    return commit_id_;
  }

private:

  engine::BufferPointer buffer_;         // Buffer received for this item

  size_t pop_id_;                        // Identifier of this particular item
  size_t block_id_;                      // Identifier of the block
  size_t commit_id_;                     // Commit id where this block was added to the queue
  size_t worker_id_;                     // worker selected to retrieve this block

  au::Cronometer cronometer_;            // Cronometer since request was sent to the worker
  bool worker_confirmation_;             // Confirmation from worker

  PopDelilahComponent *pop_delilah_component_;
};

class PopDelilahComponent : public DelilahComponent, public engine::NotificationListener {
public:

  PopDelilahComponent(std::string _queue, std::string _fileName, bool force_flag, bool show_flag);
  ~PopDelilahComponent();

  // Methods of DelilahComponent
  virtual void run();
  virtual void receive(const PacketPointer& packet);
  virtual void review();

  // Function to get the status
  std::string getStatus();
  std::string getExtraStatus();

  // Function to receive notifications
  void notify(engine::Notification *notification);

  // Function to check what to do with finish items
  void check();

  bool started() const {
    return started_;
  }

  void set_started(bool value) {
    started_ = value;
  }

private:

  /**
   * \brief Send main request to get connected to a queue
   */

  void SendMainRequest();

  /**
   * \brief Send request for a particular item
   */

  void SendRequest(PopDelilahComponentItem *item) const;

  // Initial information of this pop operation
  std::string queue_;      // Name of the queue we are recovering
  std::string file_name_;  // Name of the file to create
  bool force_flag_;        // Flag to remove previos directory
  bool show_flag_;

  // Main request information
  size_t worker_id_;       // worker selected to request information about my queue
  size_t commit_id_;       // last commit observed
  au::Cronometer cronometer_;  // Time since request was sent

  // Number of pop_responses
  int num_pop_queue_responses_;

  // Total number of blocks downloaded
  int num_blocks_downloaded_;

  // Information about selected queue ( contained in pop request response message )
  gpb::Queue *gpb_queue_;

  // counter for items
  size_t item_id_;

  // List of items for this pop operation
  au::map< size_t, PopDelilahComponentItem > items_;

  // Pending write operations
  int num_pending_write_operations_;

  // Flag to avoid finishing component before it was started
  bool started_;
};
}

#endif  // ifndef _H_POP_DELILAH_COMPONENT

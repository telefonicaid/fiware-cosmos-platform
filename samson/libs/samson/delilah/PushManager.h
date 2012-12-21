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


#ifndef _H_SAMSON_PUSH_MANAGER
#define _H_SAMSON_PUSH_MANAGER

#include "au/containers/map.h"
#include "au/tables/Table.h"


namespace samson {
class Delilah;
class Packet;

class PushItem {
public:

  PushItem(Delilah *delilah, size_t item_id, engine::BufferPointer buffer, const std::vector<std::string>& queues);

  void Review();                // Review this item ( send again if worker is down....
  bool IsFinished();            // Is completed
  void ResetPushItem();         // Reest this push operation

  // Accessors
  size_t push_id() const;
  size_t time() const;
  size_t size() const;

  // Messages recevied form workers
  void receive(Message::MessageCode mdgCode, size_t worker_id, au::ErrorManager& error);

  // Send commit message ( only in ready_for_commit state )
  void SendCommit();

  // Get a description of current status
  std::string str() const;
  std::string str_buffer_info() const;

private:

  typedef enum {
    init,
    waiting_push_block_response,       // Waiting for a push response message
    waiting_push_block_confirmation,   // Waiting for final confirmation to remove this block in delilah
    completed
  } PushItemState;

  size_t push_id_;                  // Identifier of this push
  engine::BufferPointer buffer_;    // Buffer considered in this push

  // Queues to send data
  std::vector<std::string> queues_;

  // Information about SAMSON push try
  size_t worker_id_;                // Worker_id used for this push
  au::Cronometer cronometer_;       // Time since I send packet

  // Internal state
  PushItemState state_;

  // Pointer to delilah client to send packets and check for workers
  Delilah *delilah_;

  friend class PushManager;
};

class PushManager {
public:

  PushManager(Delilah *delilah);

  // Main function to push data to SAMSON
  size_t Push(engine::BufferPointer buffer, const std::vector<std::string>& queues);

  // Reset all elements
  void ResetAllItems();

  // Receive this packet from worker
  void receive(Message::MessageCode mdgCode, size_t worker_id, size_t push_id, au::ErrorManager& error);

  // General review function
  void Review();

  // Get table of collections
  au::tables::Table *getTableOfItems();

  // Get the number of pushing elments
  size_t GetPendingSizeToPush() const;

public:

  au::map<size_t, PushItem> items_;               // Items currently uploading to the cluster
  size_t item_id_;                                // Next identifier for a push item
  Delilah *delilah_;                              // Pointer to delilah to send and receive packets
};
}  // end of namespace samson

#endif  // ifndef _H_SAMSON_PUSH_MANAGER

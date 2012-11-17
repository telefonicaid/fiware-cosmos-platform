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
#ifndef _H_SAMSON_BLOCK_REQUEST
#define _H_SAMSON_BLOCK_REQUEST

#include "au/containers/Uint64Vector.h"
#include "au/statistics/Cronometer.h"
#include "au/string/Descriptors.h"
#include "samson/common/KVHeader.h"
#include "samson/common/MessagesOperations.h"
#include "samson/common/samson.pb.h"
#include "samson/stream/Block.h"
#include <set>
#include <string>


namespace samson {
class SamsonWorker;

class BlockRequest {
public:

  BlockRequest(SamsonWorker *samson_worker, size_t block_id);
  ~BlockRequest() {
  }

  /**
   * \brief Review this request
   */
  void Review();

  /**
   * \brief Notification that an error has been received from a wokre
   */
  void NotifyErrorMessage(size_t worker_id, const std::string& error_message);

  /**
   * \brief Get information for delilah listing
   */
  void fill(gpb::CollectionRecord *record, const Visualization& visualization);

  bool finished() {
    return finished_;
  }

  size_t block_id() {
    return block_id_;
  }

  std::string pattern_name() {
    return str_block_id(block_id_);
  }

  size_t creation_time() {
    return cronometer_.seconds();
  }

private:

  SamsonWorker *samson_worker_;       // Pointer to samsonWorker to interact with network, worker_controller,...

  size_t block_id_;                   // Identifier of this block
  size_t worker_id_;                  // Worker where we have sent a request ( -1 if nothing has been sent )

  au::Cronometer cronometer_;         // Cronometer since creation ( to reset )
  au::Cronometer last_request_cronometer_;

  au::Descriptors sent_packets_;       // Statistics about workers asked for this packet
  au::Descriptors fails_;             // Statistics about workers asked for this packet

  std::list<size_t> worker_ids_;       // List of worker ids to test

  bool finished_;                     // Flag to indicate that block is already local

  std::string state_;                 // Status string updated in the last "Reset"
};
}

#endif  // ifndef _H_SAMSON_BLOCK_REQUEST
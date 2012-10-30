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

#include "samson/stream/BlockManager.h"
#include "samson/worker/SamsonWorker.h"

#include "BlockRequest.h" // Own interface

namespace samson {

  
  BlockRequest::BlockRequest(SamsonWorker *samson_worker, size_t block_id, size_t worker_id) {
    samson_worker_ = samson_worker;
    block_id_ = block_id;
    worker_id_ = worker_id;
    
    
    // Send packet to selected worker
    PacketPointer packet(new Packet(Message::BlockRequest));
    packet->message->set_block_id(block_id_);
    packet->to = NodeIdentifier(WorkerNode, worker_id_);
    
    // Send packet
    samson_worker_->network()->Send(packet);
  }
}
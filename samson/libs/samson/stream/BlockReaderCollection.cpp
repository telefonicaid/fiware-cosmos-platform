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
#include "samson/stream/BlockReaderCollection.h"

namespace samson {
namespace stream {
au::SharedPointer<BlockReader> BlockReader::create(BlockRef *block_ref, int channel, au::ErrorManager& error) {

  // Candidate instance
  au::SharedPointer<BlockReader> block_reader(new BlockReader());

  block_reader->channel_ = channel;

  // Get pointer to the block
  BlockPointer block = block_ref->block();

  if (!block->is_content_in_memory()) {
    error.set("Content of block is not in memory");
    return au::SharedPointer<BlockReader>(NULL);
  }

  // Get the KVFile
  block_reader->kv_file_ = block_ref->file();

  if (error.IsActivated()) {
    return au::SharedPointer<BlockReader>(NULL);
  }

  return block_reader;
}

void BlockReaderCollection::AddInputBlocks(BlockRef *block_ref, int channel) {
  if ((channel < 0) || (channel > (operation_->getNumInputs() - 1))) {
    LM_X(1, ("Internal error"));
  }
  au::ErrorManager error;
  au::SharedPointer<BlockReader> block_reader = BlockReader::create(block_ref, channel, error);

  if (error.IsActivated()) {
    // Still not supported how to handle this error nicely
    LM_X(1, ("Error crearing a block reader: %s", error.GetMessage().c_str()));
  }
  input_block_readers_.push_back(block_reader);
}

void BlockReaderCollection::AddStateBlocks(BlockRef *block_ref, int channel) {
  if ((channel < 0) || (channel > (operation_->getNumInputs() - 1))) {
    LM_X(1, ("Internal error"));
  }
  au::ErrorManager error;
  au::SharedPointer<BlockReader> block_reader = BlockReader::create(block_ref, channel, error);

  if (error.IsActivated()) {
    // Still not supported how to handle this error nicely
    LM_X(1, ("Error crearing a block reader: %s", error.GetMessage().c_str()));
  }
  state_block_readers_.push_back(block_reader);
}

size_t BlockReaderCollection::PrepareProcessingHashGroup(int hg) {

  // Getting the number of key-values for input
  input_num_kvs_ = 0;
  for (size_t i = 0; i < input_block_readers_.size(); i++) {
    input_num_kvs_ += input_block_readers_[i]->kv_file()->info[hg].kvs;
  }

  // Getting the number of key-values for state
  state_num_kvs_ = 0;
  for (size_t i = 0; i < state_block_readers_.size(); i++) {
    state_num_kvs_ += state_block_readers_[i]->kv_file()->info[hg].kvs;
  }

  // Total number of key-values
  size_t total_kvs = input_num_kvs_ + state_num_kvs_;

  // Prepare KV Vector with the total number of kvs ( from input and state )
  kvVector_.prepareInput(total_kvs);

  // Add key values for all the inputs
  if (input_num_kvs_ > 0) {
    for (size_t i = 0; i < input_block_readers_.size(); i++) {
      kvVector_.addKVs(input_block_readers_[i]->channel(), input_block_readers_[i]->kv_file()->info[hg],
                       input_block_readers_[i]->kv_file()->kvs_for_hg(hg));
    }
  }

  // Add key values for the state
  if (state_num_kvs_ > 0) {
    for (size_t i = 0; i < state_block_readers_.size(); i++) {
      kvVector_.addKVs(state_block_readers_[i]->channel(), state_block_readers_[i]->kv_file()->info[hg],
                       state_block_readers_[i]->kv_file()->kvs_for_hg(hg));
    }
  }

  if (total_kvs > 0) {
    // Sort and init KVVector

    if (state_num_kvs_ == 0) {
      kvVector_.sort(); // General sort
    } else if (input_num_kvs_ > 0) {
      kvVector_.sortAndMerge(input_num_kvs_); // Sort first part, merge global
    }
    kvVector_.Init();
  }
  return total_kvs;
}

KVSetStruct *BlockReaderCollection::GetNext() {
  return kvVector_.GetNext();
}
}
}

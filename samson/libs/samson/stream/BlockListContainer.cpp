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
#include "samson/stream/BlockListContainer.h"  // Own interface

#include <list>

#include "samson/stream/Block.h"              // Block
#include "samson/stream/BlockList.h"          // BlockList

namespace samson {
namespace stream {
BlockListContainer::BlockListContainer(std::string container_name, size_t task_id) {
  container_name_ = container_name;
  task_id_ = task_id;
}

BlockListContainer::~BlockListContainer() {
  blockLists_.clearMap();   // Remove all BlockList instances
}

BlockList *BlockListContainer::findBlockList(std::string name) const {
  au::TokenTaker tt(&token_);

  return blockLists_.findInMap(name);
}

BlockList *BlockListContainer::getBlockList(std::string name) {
  au::TokenTaker tt(&token_);
  BlockList *blockList = blockLists_.findInMap(name);

  if (!blockList) {
    blockList = new BlockList(au::str("<%s:%s>", container_name_.c_str(), name.c_str()), task_id_);
    blockLists_.insertInMap(name, blockList);
  }

  return blockList;
}

void BlockListContainer::clearBlockListcontainer() {
  au::TokenTaker tt(&token_);

  blockLists_.clearMap();   // Remove all BlockList instances
}

std::vector<std::string> BlockListContainer::get_block_list_names() {
  au::TokenTaker tt(&token_);

  std::vector<std::string> keys;
  au::map<std::string, BlockList>::iterator it_blockLists;
  for (it_blockLists = blockLists_.begin(); it_blockLists != blockLists_.end(); it_blockLists++) {
    keys.push_back(it_blockLists->first);
  }
  return keys;
}

size_t BlockListContainer::getNumBlocks() {
  au::TokenTaker tt(&token_);
  size_t total = 0;

  au::map<std::string, BlockList>::iterator it;
  for (it = blockLists_.begin(); it != blockLists_.end(); it++) {
    total += (it->second->getNumBlocks());
  }
  return total;
}

bool BlockListContainer::is_content_in_memory() {
  au::TokenTaker tt(&token_);

  au::map<std::string, BlockList>::iterator it;
  for (it = blockLists_.begin(); it != blockLists_.end(); it++) {
    BlockList *block_list = it->second;
    if (!block_list->IsContentInMemory()) {
      return false;
    }
  }
  return true;
}

void BlockListContainer::lock_content_in_memory() {
  au::TokenTaker tt(&token_);

  au::map<std::string, BlockList>::iterator it;
  for (it = blockLists_.begin(); it != blockLists_.end(); it++) {
    BlockList *block_list = it->second;
    block_list->lock_content_in_memory();
  }
}

std::string BlockListContainer::str_blocks() const {
  au::TokenTaker tt(&token_);
  std::ostringstream output;

  au::map<std::string, BlockList>::const_iterator it;
  for (it = blockLists_.begin(); it != blockLists_.end(); it++) {
    BlockList *block_list = it->second;
    output << "<<" << it->first << " " << block_list->str_blocks() << ">> ";
  }
  return output.str();
}

std::string BlockListContainer::str_block_ids() const {
  au::TokenTaker tt(&token_);
  std::ostringstream output;
  int num_inputs = 0;

  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("input_%d", i))) {
      num_inputs = i + 1;
    }
  }

  int num_outputs = 0;
  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("output_%d", i))) {
      num_outputs = i + 1;
    }
  }

  for (int i = 0; i < num_inputs; ++i) {
    BlockList *block_list = blockLists_.findInMap(au::str("input_%d", i));
    if (!block_list) {
      output << "-";
    } else {
      output << block_list->str_blocks();
    }
  }

  for (int i = 0; i < num_outputs; i++) {
    BlockList *block_list = blockLists_.findInMap(au::str("output_%d", i));
    if (!block_list) {
      output << "-";
    } else {
      output << block_list->str_blocks();
    }
  }

  return output.str();
}

std::string BlockListContainer::str_inputs() const {
  au::TokenTaker tt(&token_);
  int num_inputs = 0;

  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("input_%d", i))) {
      num_inputs = i + 1;
    }
  }

  std::ostringstream output;
  for (int i = 0; i < num_inputs; ++i) {
    BlockList *block_list = blockLists_.findInMap(au::str("input_%d", i));
    if (!block_list) {
      output << "-";
    }
    BlockInfo block_info = block_list->getBlockInfo();
    output << "[" << block_info.strShortInfo() << "]";
  }
  return output.str();
}

std::string BlockListContainer::str_outputs() const {
  au::TokenTaker tt(&token_);
  int num_outputs = 0;

  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("output_%d", i))) {
      num_outputs = i + 1;
    }
  }

  std::ostringstream output;
  for (int i = 0; i < num_outputs; i++) {
    BlockList *block_list = blockLists_.findInMap(au::str("output_%d", i));
    if (!block_list) {
      output << "-";
    }
    BlockInfo block_info = block_list->getBlockInfo();
    output << "[" << block_info.strShortInfo() << "]";
  }
  return output.str();
}

FullKVInfo BlockListContainer::GetInputsInfo() const {
  au::TokenTaker tt(&token_);
  int num_inputs = 0;

  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("input_%d", i))) {
      num_inputs = i + 1;
    }
  }
  FullKVInfo info;
  for (int i = 0; i < num_inputs; ++i) {
    BlockList *block_list = blockLists_.findInMap(au::str("input_%d", i));
    if (block_list) {
      BlockInfo block_info = block_list->getBlockInfo();
      info.append(block_info.info);
    }
  }
  return info;
}

FullKVInfo BlockListContainer::GetOutputsInfo() const {
  au::TokenTaker tt(&token_);
  int num_outputs = 0;

  for (int i = 0; i < 10; i++) {
    if (blockLists_.findInMap(au::str("output_%d", i))) {
      num_outputs = i + 1;
    }
  }

  FullKVInfo info;
  for (int i = 0; i < num_outputs; ++i) {
    BlockList *block_list = blockLists_.findInMap(au::str("output_%d", i));
    if (block_list) {
      BlockInfo block_info = block_list->getBlockInfo();
      info.append(block_info.info);
    }
  }
  return info;
}
}
}

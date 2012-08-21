

#include "Block.h"              // Block
#include "BlockList.h"          // BlockList

#include "BlockListContainer.h"  // Own interface

namespace samson {
namespace stream {
BlockListContainer::BlockListContainer(std::string _container_name) {
  container_name = _container_name;
}

BlockListContainer::~BlockListContainer() {
  blockLists.clearMap();                      // Remove all BlockList instances
}

BlockList *BlockListContainer::getBlockList(std::string name) {
  BlockList *blockList = blockLists.findInMap(name);

  if (!blockList) {
    blockList = new BlockList(au::str("<%s:%s>", container_name.c_str(), name.c_str()));
    blockLists.insertInMap(name, blockList);
  }

  return blockList;
}

void BlockListContainer::clearBlockListcontainer() {
  blockLists.clearMap();                      // Remove all BlockList instances
}

std::vector<std::string> BlockListContainer::get_block_list_names() {
  std::vector<std::string> keys;
  au::map<std::string, BlockList >::iterator it_blockLists;
  for (it_blockLists = blockLists.begin(); it_blockLists != blockLists.end(); it_blockLists++) {
    keys.push_back(it_blockLists->first);
  }
  return keys;
}

size_t BlockListContainer::getNumBlocks() {
  size_t total = 0;

  au::map<std::string, BlockList >::iterator it;
  for (it = blockLists.begin(); it != blockLists.end(); it++) {
    total += (it->second->getNumBlocks());
  }
  return total;
}

bool BlockListContainer::is_content_in_memory() {
  au::map<std::string, BlockList >::iterator it;
  for (it = blockLists.begin(); it != blockLists.end(); it++) {
    BlockList *block_list = it->second;

    au::list< BlockRef >::iterator it_blocks;                // List of blocks references
    for (it_blocks = block_list->blocks.begin(); it_blocks != block_list->blocks.end(); it_blocks++) {
      BlockRef *block_ref = *it_blocks;
      BlockPointer block = block_ref->block();
      if (!block->is_content_in_memory())
        return false;
    }
  }
  return true;
}

void BlockListContainer::lock_content_in_memory() {
  au::map<std::string, BlockList >::iterator it;
  for (it = blockLists.begin(); it != blockLists.end(); it++) {
    BlockList *block_list = it->second;
    block_list->lock_content_in_memory();
  }
}
}
}

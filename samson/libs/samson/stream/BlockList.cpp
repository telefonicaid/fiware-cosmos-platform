
#include <sstream>

#include "logMsg/logMsg.h"         // LM_M

#include "au/file.h"               // au::sizeOfFile

#include "engine/ProcessManager.h"  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/SamsonSetup.h"
#include "samson/common/coding.h"  // KVInfo

#include "samson/module/ModulesManager.h"

#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"

#include "Block.h"                 // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager
#include "WorkerTask.h"

#include "BlockList.h"             // Own interface


namespace samson {
namespace stream {

BlockList::~BlockList() {
  // Make sure I am not in any list in the blocks I am retaining...
  au::list< BlockRef >::iterator it_blocks;                // List of blocks references
  for (it_blocks = blocks.begin(); it_blocks != blocks.end(); it_blocks++) {
    BlockRef *block_ref = *it_blocks;
    BlockPointer block = block_ref->block();

    block->remove_block_list(this);
  }


  clearBlockList();
}

void BlockList::clearBlockList() {
  // Remove all reference contained in this list
  blocks.clearList();
}

void BlockList::add(BlockRef *block_ref) {
  // Insert this block in my list
  blocks.push_back(block_ref);

  // Update information in the block ( for sorting )
  block_ref->block()->add_block_list(this);
}

void BlockList::remove(BlockRef *block_ref) {
  // Remove this block from my list of blocks
  blocks.remove(block_ref);

  // Update information in the block ( for sorting )
  block_ref->block()->remove_block_list(this);
}

void BlockList::lock_content_in_memory() {
  au::list< BlockRef >::iterator it_blocks;                // List of blocks references
  for (it_blocks = blocks.begin(); it_blocks != blocks.end(); it_blocks++) {
    BlockRef *block_ref = *it_blocks;
    BlockPointer block = block_ref->block();
    if (!block->is_content_in_memory()) {
      LM_X(1, ("Internal error"));
    }
    block->lock_content_in_memory(this);
  }
}

size_t BlockList::getNumBlocks() {
  return blocks.size();
}

size_t BlockList::task_id() {
  return task_id_;
}

int BlockList::priority() {
  return priority_;
}

BlockInfo BlockList::getBlockInfo() {
  BlockInfo block_info;

  au::list< BlockRef >::iterator it;
  for (it = blocks.begin(); it != blocks.end(); it++) {
    BlockRef *block_ref = *it;
    block_ref->append(block_info);
  }

  return block_info;
}

void BlockList::ReviewBlockReferences(au::ErrorManager& error) {
  au::list< BlockRef >::iterator it;
  for (it = blocks.begin(); it != blocks.end(); it++) {
    (*it)->review(error);
    if (error.IsActivated()) {
      return;
    }
  }
}

std::string BlockList::str_blocks() {
  if (blocks.size() == 0) {
    return "empty";
  }

  std::ostringstream output;
  output << "[ ";
  au::list< BlockRef >::iterator it;
  for (it = blocks.begin(); it != blocks.end(); it++) {
    output << (*it)->block_id() << " ";
  }
  output << "]";
  return output.str();
}
}
}


#ifndef _H_BLOCK_LIST_CONTAINER
#define _H_BLOCK_LIST_CONTAINER

/* ****************************************************************************
 *
 * FILE                      BlockListContainer.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Container of BlockLists
 *
 */

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "au/containers/map.h"                      // au::map
#include "samson/stream/BlockInfo.h"                // struct BlockInfo
#include "samson/stream/BlockList.h"

namespace samson {
namespace stream {
class BlockList;
class Block;

/*
 *
 * Collection of blockLists
 *
 * Used in WorkerTaskBase to contain all the references to blocks included in operations ( input & output )
 */

class BlockListContainer {
  public:
    // Constructor and destructor
    BlockListContainer(std::string _container_name, size_t task_id);
    ~BlockListContainer();

    // Get a particular block list
    BlockList *getBlockList(std::string name);

    // Remove all block lists contained here
    void clearBlockListcontainer();

    // Get the list of names for all the block lists...
    std::vector<std::string> get_block_list_names();

    // Get the total number of blocks included in all the block lists
    size_t getNumBlocks();

    // Check if all blocks are on memory
    bool is_content_in_memory();

    // Lock content on memory
    void lock_content_in_memory();

  std::string str_inputs() const {
    
    int num_inputs = 0;
    for (int i = 0; i < 10; i++)
      if( blockLists_.findInMap(au::str("input_%d", i)))
        num_inputs = i+1;

    std::ostringstream output;
    for (int i = 0; i < num_inputs; i++) {
      BlockList *block_list = blockLists_.findInMap(au::str("input_%d", i));
      if( !block_list )
        output << "-";
      BlockInfo block_info = block_list->getBlockInfo();
      output << "[" << block_info.strShortInfo() << "]";
    }
    return output.str();
  }
  
  std::string str_outputs() const {
    
    int num_outputs = 0;
    for (int i = 0; i < 10; i++)
      if( blockLists_.findInMap(au::str("output_%d", i)))
        num_outputs = i+1;
    
    std::ostringstream output;
    for (int i = 0; i < num_outputs; i++) {
      BlockList *block_list = blockLists_.findInMap(au::str("output_%d", i));
      if( !block_list )
        output << "-";
      BlockInfo block_info = block_list->getBlockInfo();
      output << "[" << block_info.strShortInfo() << "]";
    }
    return output.str();
  }
  
  
  private:
    size_t task_id_;
    std::string container_name_;
    au::map<std::string, BlockList> blockLists_;
};
}
}

#endif  // ifndef _H_BLOCK_LIST_CONTAINER

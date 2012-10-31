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

  private:
    size_t task_id_;
    std::string container_name_;
    au::map<std::string, BlockList> blockLists_;
};
}
}

#endif  // ifndef _H_BLOCK_LIST_CONTAINER

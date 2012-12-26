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

#include <sstream>
#include <string>

#include "au/containers/map.h"                         // au::map

#include "BlockInfo.h"                              // struct BlockInfo

namespace samson 
{
    namespace stream
    {
        class BlockList;
        class Block;
        
        class BlockListContainer
        {
            std::string container_name;
            
        protected:
            
            au::map<std::string, BlockList > blockLists;
            
        public:

            BlockListContainer( std::string _container_name );
            ~BlockListContainer();
            
            BlockList* getBlockList( std::string name );
            
            void copyFrom( BlockListContainer* other );
            
            void clearBlockListcontainer();
            
            // Get the information about contained blocks considering them only one if there are contained in multiple BlockLists    
            BlockInfo getUniqueBlockInfo();
            
            // Check is a particular block is included here
            bool isBlockIncluded( Block* block );
          
            // Get the list of names for all the block lists...
            std::vector<std::string> get_block_list_names();
            
            // Get a description of the contents
            std::string getBlockListContainerDataDescription();
            
            // Get the total number of blocks included in all the block lists
            size_t getNumBlocks();
            
        };
    }
}
        
#endif
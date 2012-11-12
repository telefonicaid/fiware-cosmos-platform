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


#include "BlockList.h"          // BlockList
#include "Block.h"              // Block

#include "BlockListContainer.h" // Own interface

namespace samson
{
    namespace stream
    {
        BlockListContainer::BlockListContainer( std::string _container_name )
        {
            container_name = _container_name;
        }
        
        BlockListContainer::~BlockListContainer()
        {
            blockLists.clearMap();                  // Remove all BlockList instances
        }
        
        BlockList* BlockListContainer::getBlockList( std::string name )
        {
            BlockList *blockList = blockLists.findInMap( name );
            
            if( !blockList )
            {
                blockList = new BlockList( au::str("<%s:%s>" , container_name.c_str() , name.c_str() ) );
                blockLists.insertInMap(name, blockList);    
            }
            
            return blockList;
        }
        
        void BlockListContainer::copyFrom( BlockListContainer* other )
        {
            au::map<std::string, BlockList >::iterator it_blockLists;
            for ( it_blockLists = other->blockLists.begin() ; it_blockLists != other->blockLists.end() ; it_blockLists++ )
                getBlockList( it_blockLists->first )->copyFrom( it_blockLists->second );
        }
        
        void BlockListContainer::clearBlockListcontainer()
        {
            blockLists.clearMap();                  // Remove all BlockList instances
        }

        // Get the information about contained blocks considering them only one if there are contained in multiple BlockLists    
        BlockInfo BlockListContainer::getUniqueBlockInfo()
        {
            // Get all blocks ( only once )
            std::set< Block* > blocks;
            
            au::map<std::string, BlockList >::iterator it_blockLists;
            for ( it_blockLists = blockLists.begin() ; it_blockLists != blockLists.end() ; it_blockLists++ )
            {
                BlockList *block_list = it_blockLists->second;
             
                au::list< Block >::iterator it_blocks;
                for (it_blocks = block_list->blocks.begin() ; it_blocks != block_list->blocks.end() ; it_blocks++ )
                    blocks.insert( *it_blocks ); 
            }

            // Collect all information
            BlockInfo block_info;
            std::set< Block*>::iterator it_blocks;
            for (it_blocks = blocks.begin() ; it_blocks != blocks.end() ; it_blocks++ )
            {
                Block* block = *it_blocks;
                block->update( block_info );
            }
            
            return block_info;
        }
        
        
        bool BlockListContainer::isBlockIncluded( Block* block )
        {
            au::map<std::string, BlockList >::iterator it_blockLists;
            for ( it_blockLists = blockLists.begin() ; it_blockLists != blockLists.end() ; it_blockLists++ )
            {
                BlockList *block_list = it_blockLists->second;
                if( block_list->isBlockIncluded( block ) )
                    return true;
            }
            return false;
        }
        
        std::vector<std::string> BlockListContainer::get_block_list_names()
        {
            std::vector<std::string> keys;
            au::map<std::string, BlockList >::iterator it_blockLists;
            for ( it_blockLists = blockLists.begin() ; it_blockLists != blockLists.end() ; it_blockLists++ )
                keys.push_back(it_blockLists->first);
            return keys;                
        }
        
        std::string BlockListContainer::getBlockListContainerDataDescription()
        {
            std::ostringstream output;
            output << "[ ";
            au::map<std::string, BlockList >::iterator it_blockLists;
            for ( it_blockLists = blockLists.begin() ; it_blockLists != blockLists.end() ; it_blockLists++ )
            {
                output << it_blockLists->first;
                output << " : ";
                output << it_blockLists->second->getBlockInfo().strShort();
                output << " ";
                
            }
            output << "]";
            
            return output.str();
        }

        size_t BlockListContainer::getNumBlocks()
        {
            size_t total = 0;
            au::map<std::string, BlockList >::iterator it;
            for (it = blockLists.begin() ; it != blockLists.end() ; it++ )
                total += (it->second->getNumBlocks() );
            return total;
        }

    } 
}
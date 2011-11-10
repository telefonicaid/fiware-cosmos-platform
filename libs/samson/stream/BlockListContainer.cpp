

#include "BlockList.h"          // BlockList
#include "Block.h"              // Block

#include "BlockListContainer.h" // Own interface

namespace samson
{
    namespace stream
    {
        BlockListContainer::~BlockListContainer()
        {
            blockLists.clearMap();                  // Remove all BlockList instances
        }
        
        BlockList* BlockListContainer::getBlockList( std::string name )
        {
            BlockList *blockList = blockLists.findInMap( name );
            
            if( !blockList )
            {
                blockList = new BlockList( name );
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
        
        
        
    } 
}
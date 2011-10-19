

#include "BlockList.h"          // BlockList

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
        
        
    } 
}
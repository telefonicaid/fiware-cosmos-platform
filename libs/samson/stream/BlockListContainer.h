
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
            
        };
    }
}
        
#endif
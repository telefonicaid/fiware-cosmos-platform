
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

#include "au/map.h"                         // au::map


namespace samson 
{
    namespace stream
    {
        class BlockList;
        
        class BlockListContainer
        {
            au::map<std::string, BlockList > blockLists;
            
        public:
            
            ~BlockListContainer();
            
            BlockList* getBlockList( std::string name );
            
            void copyFrom( BlockListContainer* other );
            
            void clearBlockListcontainer();
            
        };
    }
}
        
#endif
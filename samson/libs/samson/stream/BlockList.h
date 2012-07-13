#ifndef _H_BLOCK_LIST
#define _H_BLOCK_LIST

/* ****************************************************************************
 *
 * FILE                      BlockList.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * List of blocks: A tool for managing groups of blocks in queues, queue_operations, states, etc..
 *
 */


#include <ostream>              // std::ostream
#include <string>               // std::string

#include "au/containers/list.h"            // au::list
#include "au/containers/map.h"             // au::map
#include "au/Cronometer.h"      // au::cronometer

#include "engine/Buffer.h"      // engine::Buffer
#include "engine/BufferContainer.h"
#include "engine/Object.h"                  // engine::Object

#include "samson/common/coding.h"           // FullKVInfo
#include "samson/common/samson.pb.h"        // network::

#include "samson/stream/BlockInfo.h"        // BlockInfo



/**
 
 BlockList: container Blocks.
    
    Note:   All the nodes are contained in at least one BlockList.
            If a block is not contained in any of them, it is automatically removed from the Block Manager
 
 */

namespace samson {
    
    class Info;
    
    namespace stream
    {
        class Block;
        class Queue;
        class StreamManager;
        class QueueItem;
        class BlockMatrix;
        
        class BlockList
        {

            BlockInfo accumulated_block_info;       // Accumulated information
            
        public:
            
            std::string name;                       // Name of this block list ( for debugging )
            size_t task_id;                         // Order of the task if really a task
            bool lock_in_memory;                    // Lock in memory
            int priority;                           // Priority level for blocks that are not involved in tasks
            bool queue;                             // Flag to indicate that this list is a queue
            
            au::list< Block > blocks;               // List of blocks

            BlockList( std::string _name = "no_name" , size_t _task_id = ( size_t) - 1 , bool _lock_in_memory = false )
            {
                name = _name;
                task_id             = _task_id;             // Task is the order of priority
                lock_in_memory      = _lock_in_memory;      // By default no lock in memory
                priority = 0;                               // Default priority level
                queue = false;                              // By default this is not a queue
            }
            
            ~BlockList();
            
            // Create a block for this BlockList
            Block* createBlock( engine::Buffer *buffer );
            Block* createBlock( engine::BufferListContainer *buffer_list_container );

            // Create a block for this BlockList
            Block* createBlockFromFile( std::string fileName );
            
            // Get the next element to be processed
            Block* top( );
            
            // Get next block for defrag ( minimum hash-group )
            Block* getNextBlockForDefrag();
            
            // Simple add or remove blocks
            void add( Block *b );
            void remove( Block* block );

            // remove a particular block with this id
            void remove( size_t id );
            void remove( BlockList* list );
            
            // get a block with this id ( if included in this list )
            Block* getBlock( size_t id );
            
            // Remove all the blocks contained in the list
            void clearBlockList();
            
            // Replace block scontained in from for blocks contained in to ( only if this block list contains all blocks contained in "from" )
            void replace( BlockList *from , BlockList *to);
            
            // Get information
            size_t getSize();
            
            bool isEmpty();
            
            bool isContentOnMemory();
            
            // Get information about this block
            void update( BlockInfo &block_info);
            BlockInfo getBlockInfo();

            size_t getNumBlocks();
            
            //void copyFrom( BlockMatrix* matrix , int channel );
            void copyFrom( BlockList* list );
            void copyFrom( BlockList* list , size_t max_size );
            void copyFrom( BlockList* list , KVRange range );
            void copyFrom( BlockList* list , KVRange range , bool exclusive , size_t max_size );
            void copyFirstBlockFrom(BlockList* list, int hashgroup);

            // Extract blocks of data
            void extractFrom( BlockList* list , size_t max_size = 0 );
            void extractFromForDefrag( BlockList* list , size_t max_size = 0 );
            bool extractBlockFrom( BlockList *list );
            
            // Get information for monitoring
            void getInfo( std::ostringstream& output);
            
            // string containing all block_ids ( only debugging )
            std::string strBlockIds();
            
            // Check if all the blocks are contained in this range
            bool isContained( KVRange range );
            
            // Operations with block ids
            void addBlockIdsTo( std::set<size_t> &block_ids );
            void removeBlockIdsAt( std::set<size_t> &block_ids );
            bool isAnyBlockIncludedIn( std::set<size_t> &block_ids);

            // Get a unit of defragmentation
            double getFragmentationFactor();
            
            //Debugging str
            std::string strRanges();
            std::string strShortDescription();
            
            // Modify prioriyt of the queue
            void setPriority(int p );

            // Set the queue flag
            void setAsQueueBlockList();
            size_t getOldestBlockTime();
            size_t getPosition(Block* b);
            
            // Check if a particular block is included
            bool isBlockIncluded( Block* block );

        };
        
        
    }
}

#endif

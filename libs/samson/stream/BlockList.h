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

#include "au/list.h"            // au::list
#include "au/map.h"             // au::map
#include "au/Cronometer.h"      // au::cronometer

#include "engine/Buffer.h"      // engine::Buffer

#include "samson/common/coding.h"           // FullKVInfo

#include "samson/data/SimpleDataManager.h"  // samson::SimpleDataManager

#include "samson/common/samson.pb.h"        // network::
#include "engine/Object.h"                  // engine::Object

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
            friend class Block;
            
            friend class StreamManager;
            friend class Queue;
            friend class QueueItem;
            friend class ParserQueueTask;
            friend class MapQueueTask;
            friend class ReduceQueueTask;
            friend class ParserOutQueueTask;
            friend class PopQueueTask;
            friend class BlockBreakQueueTask;
            friend class SortQueueTask;
            
            au::list< Block > blocks;               // List of blocks
            
            BlockInfo accumulated_block_info;       // Accumulated information
            
            std::string name;                       // Name of this block list ( for debugging )
            size_t task_id;                         // Order of the task if really a task
            bool lock_in_memory;                    // Lock in memory
            
        public:
                        
            BlockList( std::string _name )
            {
                name = _name;
                task_id             = (size_t) -1;      // By default minimum priority
                lock_in_memory      = false;            // By default no lock in memory                
            }

            BlockList( std::string _name , size_t _task_id , bool _lock_in_memory )
            {
                name = _name;
                task_id             = _task_id;             // Task is the order of priority
                lock_in_memory      = _lock_in_memory;      // By default no lock in memory
                
            }
            
            ~BlockList();
            
            // Create a block for this BlockList
            Block* createBlock( engine::Buffer *buffer );

            // Create a block for this BlockList
            Block* createBlockFromDisk( size_t id );
            
            // Get the next element to be processed
            Block* top( );

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
            void copyFrom( BlockList* list , KVRange range );
            
            // Extract blocks of data
            void extractFrom( BlockList* list , size_t max_size );
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
            // string containing all block_ids ( only debuggin )
            std::string strBlockIds();
            
        };
        
        
    }
}

#endif
#ifndef _H_BLOCK
#define _H_BLOCK

/* ****************************************************************************
 *
 * FILE                      Block.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * Basic unit of data for stream processing
 * A block is a unit of data possibly in memory with replication of disk
 *
 */


#include "au/simple_map.h"                         // au::simple_map

#include "engine/MemoryManager.h"

#include "samson/common/coding.h"                   

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/Object.h"                          // engien::EngineListener
#include "engine/Object.h"                          // engine::Object

#include "samson/module/KVSetStruct.h"              // samson::KVSetStruct

#include "BlockInfo.h"                              // struct BlockInfo

#include <set>

namespace samson {
    namespace stream
    {
        
        class BlockList;
        
        /**
         Main class to hold a block on memory
         */
        
        class Block :  public engine::Object
        {
            
            friend class BlockManager;  
            friend class BlockList;
            friend class PopQueueTask;
            friend class BlockBreakQueueTask;
            friend class BlockReader;
            friend class Queue;
            friend class StreamOutQueueTask;
            
            size_t id;                      // Identifier of the block ( in this node )
            
            engine::Buffer *buffer;         // Buffer of data if content of this block is on memory
            
            KVHeader* header;               // Always on memory copy of the header
            
            size_t size;                    // Size of the buffer ( Not that buffer is NULL if content is not on memory )

            std::set< BlockList* > lists;   // List where this block is contained
            
            int requests;					// Number of times block has been detected as "not in memory" when scheduling a task;

            typedef enum
            {
                on_memory,      // Initial state
                writing,        // A write order has been given
                ready,          // On disk and memory
                on_disk,        // Just on disk ( memory has been freed )
                reading         // Reading from disk to recover at memory
            } BlockState;

            BlockState state;
            
        private:
            
            // Constructor only visible in a BlockList
            Block( engine::Buffer *buffer  );
            Block( size_t _id , size_t _size , KVHeader* _header );
            
        public:
            
            ~Block();

            // Set and Get priority ( manual ordering if blocks are not assigned to tasks )
            void setPriority( int _priority );
            int getPriority();
                        
            // Check if this block is not in any list anymore
            int getNumberOfLists();

            // Function to check if this block can be removed from block manager ( basically it is not contained anywhere )
            int canBeRemoved();
                                    
        private:
            
            // Send notifications to write and read 
            void write();
            void read();
            
            // Release memory ( only if saved to disk previously )
            void freeBlock();

            bool compare( Block *b );
            
            // Get the operations
            ::engine::DiskOperation* getWriteOperation();
            ::engine::DiskOperation* getReadOperation();
            
            // Notifications
            void notify( engine::Notification* notification );
            
        public:

            // Checking each state
            bool isOnDisk();
            bool isWriting();
            bool isReading();
            bool isOnMemory();
            bool isReady();

            // Checking if content is on memory and/or on disk
            bool isContentOnMemory();
            bool isContentOnDisk();

            // Checking if content is locked ( contained in a lock - block list )
            bool isLockedInMemory();

            // Getting information
            size_t getSize();
            size_t getSizeOnMemory();
            size_t getSizeOnDisk();
            KVFormat getKVFormat();
            const char* getState();
            size_t getId();
 
            // Working with KVRanges
            KVRange getKVRange();
            bool isNecessaryForKVRange( KVRange range );
            
            // Get information about this block
            void update( BlockInfo &block_info );
            
            // Debug string    
            std::string str();

            // Xml version of the info
            void getInfo( std::ostringstream& output);
            
            // Accessing information
            KVInfo* getKVInfo();
            char *getData();
            
        private:
            
            // Get the minimum task id to get the order of the blocks
            size_t getMinTaskId();
            
        };
            
        
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif

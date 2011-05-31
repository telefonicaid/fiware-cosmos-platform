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

#include "engine/MemoryManager.h"
#include "samson/common/coding.h"
#include "engine/DiskOperation.h"       // engine::DiskOperation
#include "engine/EngineNotification.h"  // engien::EngineListener
#include "samson/module/KVSetStruct.h"	// samson::KVSetStruct

#include <set>

namespace samson {
    namespace stream
    {
        
        
        /**
         Main class to hold a block on memory
         */
        
        class Block :  public engine::NotificationListener
        {
            
            friend class BlockManager;  // Friend class to create Blocks
            int priority;               // Priority of the block ( to be reordered )
            
            engine::Buffer *buffer;     // Buffer of data if is on memory
            
            size_t size;                // Size of the buffer

            int lock_counter;           // Counter to indicate that we are currenltly using this block for processing ( should not be removed from memory )
            
            size_t id;                  // Identifier of the block ( in this node )

            std::set< size_t > tasks;   // Tasks that has this block retained to be used in that operation
            
            int retain_counter;         // General counter of retain-release ( by a task or by a queue / state )
            
            typedef enum
            {
                on_memory,      // Initial state
                writing,        // A write order has been given
                ready,          // On disk and memory
                on_disk,        // Just on disk ( memory has been freed )
                reading         // Reading from disk to recover at memory
            } BlockState;

            BlockState state;
            
        public:

            // Constructor with automatic add to the BlockManager
            Block( engine::Buffer *buffer );
            ~Block();
            
            // Set and Get priority
            void setPriority( int _priority );
            int getPriority();
            
            // Funtion for debuggin
            friend std::ostream &operator<<(std::ostream &out, const Block &b);
                        
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
            
            std::string getFileName();
            
            // Notifications
            void notify( engine::Notification* notification );
            bool acceptNotification( engine::Notification* notification );
            
        public:
            
            bool isOnDisk()
            {
                return ( state == on_disk );
            }
            
            bool isWriting()
            {
                return ( state == writing );
            }
            
            bool isOnMemory()
            {
                return (state == on_memory);
            }
            
            bool isReady()
            {
                return (state == ready);
            }
            
            bool isContentOnMemory()
            {
                return (  (state == ready ) || (state == on_memory) || ( state == writing ));
            }
            
            bool isContentOnDisk()
            {
                return (  (state == ready ) || (state == on_disk) || ( state == reading ));
            }
            
            bool isLocked()
            {
                return (lock_counter > 0);
            }
            
            size_t getSize()
            {
                return size;
            }
            
            char *getData()
            {
                if( ! isContentOnMemory() )
                    LM_X(1,("Not possible to get data for a block that is not in memory"));
                return buffer->getData();
            }
            
            
            size_t getSizeOnMemory()
            {
                if( isContentOnMemory() )
                    return size;
                else
                    return 0;
            }
            
            size_t getSizeOnDisk()
            {
                if( isContentOnDisk() )
                    return size;
                else
                    return 0;
            }
            
            KVSetStruct* getKVSetStruct();
            
            
        };
            
        
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif
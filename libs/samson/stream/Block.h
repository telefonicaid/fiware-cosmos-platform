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
#include "engine/Object.h"  // engien::EngineListener
#include "engine/Object.h"              // engine::Object

#include "samson/module/KVSetStruct.h"	// samson::KVSetStruct

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
            
            size_t id;                  // Identifier of the block ( in this node )
            
            engine::Buffer *buffer;     // Buffer of data if content of this block is on memory
            
            KVHeader* header;           // Always on memory copy of the header
            size_t size;                // Size of the buffer ( Not that buffer is NULL is content is not on memory )

            
            KVInfo *info;   // Information about  content per hash-group ( in memory if required. Only used in "state" blocks  )
            
            /*
            int priority;               // Priority of the block ( to be reordered )            
            int lock_counter;           // Counter to indicate that we are currently using this block for processing 
            std::set< size_t > tasks;       // Tasks that has this block retained to be used in that operation
            std::set< std::string > queues; // Queues currently containing this block
            int retain_counter;             // General counter of retain-release ( by a task or by a queue / state )
            */

            std::set< BlockList* > lists;   // List where this block is contained
            
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
            Block( engine::Buffer *buffer , bool txt );
            
        public:
            
            ~Block();
            
            // Check if this block is not in any list anymore
            int getNumberOfLists()
            {
                return lists.size();
            }

            int canBeRemoved()
            {
                if( lists.size() != 0)
                    return false;
                
                if( state == reading )
                    return false;
                
                if( state == writing )
                    return false;

                return true;
            }
            
            
            // Set and Get priority
            void setPriority( int _priority );
            int getPriority();
                        
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
            
        public:
            
            bool isOnDisk()
            {
                return ( state == on_disk );
            }
            
            bool isWriting()
            {
                return ( state == writing );
            }
            
            bool isReading()
            {
                return ( state == reading );
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
            
            bool isLockedInMemory();
            
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
            
            bool isNecessaryForHashGroups( int _hg_begin , int _hg_end )
            {
                if( !header )
                    LM_X(1,("Internal error managing blocks"));
                
                if( _hg_end <= (int)header->hg_begin )
                    return false;
                
                if( _hg_begin >= (int)header->hg_end )
                    return false;
                
                return true;
                
            }
            
            // Get information
            KVInfo getInfo();
            
            // Debug string    
            std::string str();

            void getInfo( std::ostringstream& output);
            
            const char* getState();
            
        private:
            
            // Get the minimum task id to get the order of the blocks
            size_t getMinTaskId();

            
        };
            
        
        // Auxiliar functions
        size_t getSize( std::set<Block*> &blocks );
        
    }
}

#endif

#include "BlockManager.h"       // Own interface
#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine

#define BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS   3
#define BLOCK_MANAGEMENT_MAX_READ_OPERATIONS   3

namespace samson {
    namespace stream {


        BlockManager *blockManager = NULL;
        
        BlockManager::BlockManager()
        {
            id = 0;
            
            // Add this object as a listener of notification_disk_operation_request_response
            listen( notification_disk_operation_request_response );

            num_writing_operations=0;     // Number of writing operations ( low priority blocks )
            num_reading_operations=0;     // Number of reading operations ( high priority blocks )
        
            memory = 0;
            
            max_memory = 300000000; //300Mb for this demo
        }
        
        void BlockManager::init()
        {
            if( blockManager )
                LM_X(1, ("Error with init BlockManager (double init)"));

            blockManager = new BlockManager();
        }
        
        void BlockManager::destroy()
        {
            if( !blockManager )
                LM_X(1, ("Error destroying a non-initialized BlockManager"));

            delete blockManager;
            blockManager = NULL;
        }

        BlockManager* BlockManager::shared()
        {
            if( !blockManager )
                LM_X(1, ("Init BlockManager befure using it"));
            return blockManager;
            
        }
        
        // Notifications
        
        void BlockManager::notify( engine::Notification* notification )
        {
            if( notification->environment.get("type", "-") == "write" )
            {
                num_writing_operations--;
                _review();
            }
            if( notification->environment.get("type", "-") == "read" )
            {
                num_reading_operations--;
                _review();
            }
            
        }
        
        void BlockManager::_review()
        {
            // Schedule new write operations ( low priority elements )
            
            if( num_writing_operations < BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS )
            {
                // Lock for new write operations...
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isOnMemory() )
                    {
                        block->write();
                        num_writing_operations++;  

                        // No continue for more writes
                        if( num_writing_operations >= BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS )
                            break;
                    }
                }
            }
            
            // Free memory if possible if we are over memory
            if( memory > max_memory )
            {
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isReady() )  // Both on disk and on memory
                    {
                        if( !block->isLocked() )
                        {
                            //LM_M(("Free block"));
                            block->freeBlock();
                            memory -= block->size;
                            
                            
                            // No continue for more frees if not necessary
                            if( memory < max_memory  )
                                break;
                        }
                    }
                }                
            }
            
            // Schedule new reads operations ( high priority elements ) if available memory
            if( num_reading_operations < BLOCK_MANAGEMENT_MAX_READ_OPERATIONS )
            {
                // Lock for new write operations...
                std::list<Block*>::iterator b;
                for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isOnDisk() ) // Nedded to be loaded
                    {
                        // Only read the block if there is memory of we can free lower priority blocks
                        if( (memory + block->size) >= max_memory )
                            _freeMemoryWithLowerPriorityBLocks( block );
                        
                        if( (memory + block->size) > max_memory )
                            break;  // It is not possible this new read
                        
                        
                        // Read the block
                        block->read();
                        memory += block->size;
                        num_reading_operations++;  
                        
                        // No continue for more writes
                        if( num_reading_operations >= BLOCK_MANAGEMENT_MAX_READ_OPERATIONS )
                            break;
                    }
                }
            }
            
            
            
        }
        
        
        void BlockManager::_freeMemoryWithLowerPriorityBLocks( Block *block )
        {
            std::list<Block*>::reverse_iterator b;
            for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
            {
                Block *block_to_be_free = *b;
             
                if( block_to_be_free == block )
                    return;     // There are no more blocks with lower priority
                
                if( block_to_be_free->isReady() )  // Both on disk and on memory
                {
                    if( !block_to_be_free->isLocked() ) // Check that is not locked ( using in a process )
                    {
                        block_to_be_free->freeBlock();
                        memory -= block_to_be_free->size;

                        if( (memory + block->size ) < max_memory )
                            return;     // Enogth memory for the block we want to read
                    }
                    
                }
            }                
            
        }
        
        bool BlockManager::acceptNotification( engine::Notification* notification )
        {
            if( notification->isName(notification_disk_operation_request_response) )
                if( notification->environment.get("target", "") == "Block")
                    return true;
            
            return true;
        }

        
        
    }
}

#include <sys/types.h>
#include <dirent.h>

#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "logMsg/traceLevels.h"

#include "BlockList.h"          // BlockList
#include "BlockManager.h"       // Own interface



#define BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS   3
#define BLOCK_MANAGEMENT_MAX_READ_OPERATIONS    3

namespace samson {
    namespace stream {

        BlockManager *blockManager = NULL;
        
        BlockManager::BlockManager()
        {
            id = 1;                       // Start blocks by 1
            worker_id = 0;                // When a different if is assigned from cluster, this should be updated with setWorkerId()
 
            num_writing_operations=0;     // Number of writing operations ( low priority blocks )
            num_reading_operations=0;     // Number of reading operations ( high priority blocks )
        
            memory = 0;
            
            max_memory = (double)SamsonSetup::shared()->getUInt64("general.memory")*0.6;  // 60% of memory for block manager
            max_memory_abs = (double)SamsonSetup::shared()->getUInt64("general.memory")*0.9;  // 90% of memory for block manager

        }
        
        BlockManager::~BlockManager()
        {
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
                LM_X(1, ("Init BlockManager before using it"));
            return blockManager;
            
        }
        
        void BlockManager::insert( Block* b )
        {
            // Insert the new block in the right position
        	LM_T(LmtBlockManager,("Inserting block with memory(%lu); max_memory(%lu), block:'%s'", memory, max_memory, b->str().c_str()));
            blocks.insert( _find_pos(b),b );
            
            // Increase the amount of memory used by all blocks
            if( b->isContentOnMemory() )
                memory += b->size;
            
            LM_T(LmtBlockManager,("Inserted block with memory(%lu); max_memory(%lu), block:'%s'", memory, max_memory, b->str().c_str()));

            // Review if new free,  write or reads are necessary
            _review();
        }
        
        void BlockManager::check( Block* b )
        {
            blocks.remove( b );
            
            // Check if the block should be removed, otherwise insert back in the list....
            
            // If it can be removed, just remove...
            if( b->canBeRemoved() )
            {
                
                // Schedule remove file
                engine::DiskOperation * operation = engine::DiskOperation::newRemoveOperation( b->getFileName() , getEngineId() );
                engine::DiskManager::shared()->add( operation );
                
                
            	LM_T(LmtBlockManager,("In check, removing block:'%s'", b->str().c_str()));
                if( b->isContentOnMemory() )
                {
                    memory -= b->size;
                    LM_T(LmtBlockManager,("Discounted memory for block with memory(%lu); max_memory(%lu), read block:'%s'", memory, max_memory, b->str().c_str()));
                }
                
                if( b->isWriting() || b->isReading() )
                    LM_X(1,("Not allowed to remove an object that is reading or writing..."));
                
                delete b;
                
            }
            else
            {
                // Insert back in the global list of blocks
            	//LM_T(LmtBlockManager,("Reinserting block with memory(%lu); max_memory(%lu), read block:'%s'", memory, max_memory, b->str().c_str()));
                blocks.insert( _find_pos( b ) , b );
            }
            
            // Review if new free, write or reads are necessary
            _review();
            
        }
        // Notifications
        
        void BlockManager::notify( engine::Notification* notification )
        {
            
            if ( notification->isName( notification_disk_operation_request_response ) )
            {
                
                std::string type = notification->environment.get("type", "-");
                
                LM_T(LmtBlockManager,("Received a disk notification with type %s" , type.c_str() ));
                
                if(  type == "write" )
                {
                    num_writing_operations--;
                    LM_T(LmtBlockManager,("Received a disk notification with type %s, num_writing_operations decremented to %d" , type.c_str(), num_writing_operations ));
                    _review();
                }
                if( type == "read" )
                {
                    num_reading_operations--;
                    LM_T(LmtBlockManager,("Received a disk notification with type %s, num_reading_operations decremented to %d" , type.c_str(), num_reading_operations ));
                    _review();
                }
                
                if( type == "remove" )
                {
                    // Nothing to do... an old block file has been removed
                }

                
            }
            else 
            {
                LM_W(("Unknown notification at BlockManager"));
            }
            
        }
        
        void BlockManager::_review()
        {
            au::ExecesiveTimeAlarm alarm("BlockManager::_review");
            
        	//LM_T(LmtBlockManager,("_review loop with %lu blocks, memory(%lu); max_memory(%lu); num_writing_operations(%d); num_reading_operations(%d)", blocks.size(), memory, max_memory, num_writing_operations, num_reading_operations));
            if( num_writing_operations < BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS )
            {
                // Lock for new write operations...
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isOnMemory() )
                    {
                    	LM_T(LmtBlockManager,("With memory(%lu); max_memory(%lu), writing block:'%s'", memory, max_memory, block->str().c_str()));
                        block->write();
                        num_writing_operations++;  
                        LM_T(LmtBlockManager,("Write block num_writing_operations incremented to %d" , num_writing_operations ));

                        // No continue for more writes
                        if( num_writing_operations >= BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS )
                            break;
                    }
                }
            }
            
            // Pointer to the last free block (if any)
            Block *freedBlock = NULL;

            // Free memory if possible if we are over memory
            if( memory > max_memory )
            {
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isReady() )  // Both on disk and on memory
                    {
                    	// Slowly, we want to forget that the block has just been read because "not in memory"
                    	if (block->requests > 0)
                    	{
                    		block->requests--;
                    		continue;
                    	}

                        if( !block->isLockedInMemory() )
                        {
                        	LM_T(LmtBlockManager,("With memory(%lu) > max_memory(%lu), Free block:'%s'", memory, max_memory, block->str().c_str()));
                            block->freeBlock();
                            memory -= block->size;
                            
                            // We keep the reference to the last freed block, because we don't want to reload it in next loop
                            //freedBlock = block;
                            

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
                    size_t max_memory_priority = max_memory;
                    
                    if (block->requests > 20)
                    {
                    	max_memory_priority = max_memory_abs;
                    }


                    // If we have reached the just freed block, we don't want to reload it (nor to go searching further)
                    if (block == freedBlock)
                    {
                    	continue;
                    }

                    if( block->isOnDisk() ) // Needed to be loaded
                    {
                    	LM_T(LmtBlockManager,("Trying to read block with memory(%lu); max_memory(%lu), block:'%s'", memory, max_memory_priority, block->str().c_str()));

                        // Only read the block if there is memory of we can free lower priority blocks
                        if( (memory + block->size) >= max_memory_priority )
                        {
                        	LM_T(LmtBlockManager,("Trying to free memory for block with memory(%lu); max_memory(%lu), block:'%s'", memory, max_memory_priority, block->str().c_str()));
                            _freeMemoryWithLowerPriorityBLocks( block );
                        }
                        
                        if( (memory + block->size) > max_memory_priority )
                        {
                        	LM_T(LmtBlockManager,("Stop trying to read block with memory(%lu); max_memory(%lu), block:'%s', because no memory", memory, max_memory_priority, block->str().c_str()));
                            break;  // It is not possible this new read
                        }
                        
                        
                        // Read the block

                        block->read();
                        memory += block->size;
                        LM_T(LmtBlockManager,("With memory(%lu); max_memory(%lu), read block:'%s'", memory, max_memory_priority, block->str().c_str()));

                        num_reading_operations++;  
                        LM_T(LmtBlockManager,("Read block num_reading_operations incremented to %d" , num_reading_operations ));
                        
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
                    continue;     // There are no more blocks with lower priority
                
                if( block_to_be_free->isReady() )  // Both on disk and on memory
                {
                   	// Slowly, we want to forget that the block has just been read because "not in memory"
                    	if (block_to_be_free->requests > 0)
                    	{
                    		block_to_be_free->requests--;
                    		continue;
                    	}

                    if( !block_to_be_free->isLockedInMemory() ) // Check that is not locked ( using in a process )
                    {
                    	LM_T(LmtBlockManager,("_freeMemoryWithLowerPriorityBLocks. With memory(%lu); max_memory(%lu), Free block:'%s'", memory, max_memory, block_to_be_free->str().c_str()));
                        block_to_be_free->freeBlock();
                        memory -= block_to_be_free->size;

                        if( (memory + block->size ) < max_memory )
                            return;     // Enough memory for the block we want to read
                    }
                    
                }
            }                
            
        }

        void BlockManager::getInfo( std::ostringstream& output)
        {
            output << "<block_manager>\n";

            au::xml_simple( output , "num_writing_operations" , num_writing_operations );
            au::xml_simple( output , "num_reading_operations" , num_reading_operations );

            au::xml_simple( output , "memory" , memory );
            au::xml_simple( output , "max_memory" , max_memory );
            
            
            au::xml_iterate_list(output, "blocks", blocks);

            //Global information
            BlockInfo block_info;
            update( block_info );
            block_info.getInfo( output );
            
            
            output << "</block_manager>\n";
        }
        
        void BlockManager::setMinimumNextId( size_t min_id)
        {
            if ( id <= min_id )
                id = ( min_id + 1 );
        }
        
        size_t BlockManager::getNextBlockId()
        {
            return id++;
        }
        
        size_t BlockManager::getWorkerId()
        {
            return worker_id;
        }
        
        // Function used in the order of blocks
        std::list<Block*>::iterator BlockManager::_find_pos( Block *b )
        {
            for (std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++)
            {
                if( b->compare(*i) )
                    return i;
            }
            return blocks.end();
        }
        
        Block* BlockManager::getBlock( size_t _id )
        {
            for ( std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++ )
                if( (*i)->id == _id )
                    return *i;
            return NULL;
        }
        
        void BlockManager::update( BlockInfo &block_info )
        {
            for (std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++ )
                (*i)->update( block_info );
            
        }
        
        network::Collection* BlockManager::getCollectionOfBlocks( Visualization* visualization )
        {
            network::Collection* collection = new network::Collection();
            collection->set_name("blocks");
            
            au::list<Block>::iterator it_blocks;
            for ( it_blocks = blocks.begin() ; it_blocks != blocks.end() ; it_blocks++)
            {
                Block* block = *it_blocks;
                block->fill( collection->add_record() , visualization );
            }            
            
            return collection;            
            

        }

        
    }
}

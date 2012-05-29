#include <sys/types.h>
#include <dirent.h>

#include "logMsg/traceLevels.h"

#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup

#include "samson/common/KVHeader.h"

#include "BlockList.h"          // BlockList
#include "BlockManager.h"       // Own interface

/*

 Note: The idea is now schedule all read and write operations until max size
       and wait for scheduled read and write goes down to 0
 
*/


#define notification_review_block_manager "notification_review_block_manager"

namespace samson {
    namespace stream {

        // Function to compare 2 blocks
        bool compare_blocks( Block*b1 , Block *b2 )
        {
            
            size_t min_task_id_1 = b1->min_task;
            size_t min_task_id_2 = b2->min_task;
            
            if( min_task_id_1 == min_task_id_2 )
            {
                // Comapre by priority
                int p1 = b1->max_priority;
                int p2 = b2->max_priority;
                
                if( p1 == p2 )
                {                
                    size_t t1 = b1->compare_time;
                    size_t t2 = b2->compare_time;
                    
                    if( t1 == t2 )
                    {
                        size_t o1 = b1->order;
                        size_t o2 = b2->order;

                        return o1 < o2;
                    }
                    else
                        return ( t1 < t2 );
                }
                else
                    return (p1 > p2);
            }
            else
                return( min_task_id_1 < min_task_id_2 );
        }        

        
        BlockManager *blockManager = NULL;
        
        BlockManager::BlockManager()
        {
            id = 1;                       // Start blocks by 1
            worker_id = 0;                // When a different if is assigned from cluster, this should be updated with setWorkerId()
 
            scheduled_write_size = 0;
            scheduled_read_size = 0;

            max_memory = (double)SamsonSetup::shared()->getUInt64("general.memory")*0.8;  // 60% of memory for block manager

            listen(notification_review_block_manager);
            engine::Engine::shared()->notify( new engine::Notification(notification_review_block_manager) , 1 );
            
        }
        
        BlockManager::~BlockManager()
        {
            // Remove the blocks contained in memory
            blocks.clearList();
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
            {
                LM_W(("Error destroying a non-initialized BlockManager"));
                return;
            }

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
        	LM_T(LmtBlockManager,("Inserting block '%s'", b->str().c_str()));

            // Insert block at the end ( will be resorted before considering reading or writing )
            blocks.push_back(b);
            
            // Review where this block goes...
            review();
        }
         
        void BlockManager::notify( engine::Notification* notification )
        {
            if ( notification->isName( notification_review_block_manager ) )
            {
                review();
            } 
            else if ( notification->isName( notification_disk_operation_request_response ) )
            {
                
                std::string type      = notification->environment.get("type", "-");
                size_t operation_size = notification->environment.getSizeT("operation_size", 0 );
                
                LM_T(LmtBlockManager,("Received a disk notification with type %s and size %s" 
                                      , type.c_str() 
                                      , au::str( operation_size ).c_str() ));
                
                if(  type == "write" )
                {
                    scheduled_write_size -= operation_size;
                    review();
                }
                else if( type == "read" )
                {
                    scheduled_read_size -= operation_size;
                    review();
                }
                else if( type == "remove" )
                {
                    // Nothing to do here...
                }
            }
            else 
            {
                LM_W(("Unknown notification at BlockManager"));
            }
            
        }
        
        void BlockManager::review()
        {
            // Get setup parameter to control review of BlockManager...
            size_t max_scheduled_write_size = SamsonSetup::shared()->getUInt64("stream.max_scheduled_write_size");
            size_t max_scheduled_read_size = SamsonSetup::shared()->getUInt64("stream.max_scheduled_read_size");
            
            // No schedule new operations until all the previous one have finished
            if( scheduled_read_size > 0 )
                return;
            if( scheduled_write_size > 0 )
                return;
            
            LM_T( LmtBlockManager , ("Reviewing block manager"));
            
            // --------------------------------------------------------------------------------
            // Sort by priorities
            // --------------------------------------------------------------------------------

            //au::ExecesiveTimeAlarm alarm("BlockManager::_review");
            {
                au::ExecesiveTimeAlarm alarm("BlockManager::sort" , 0.10 );
                
                // Update sort information for all blocks
                std::list<Block*>::iterator b;
                for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                    (*b)->update_sort_information();
                
                // Sort list of blocks according to id and min_task involved
                blocks.sort( compare_blocks );
            }
            
            // --------------------------------------------------------------------------------
            // Remove old blocks not included anywhere
            // --------------------------------------------------------------------------------
            
            LM_T( LmtBlockManager , ("Remove old blocks"));
            {
                std::list<Block*>::iterator b;
                for ( b = blocks.begin() ; b != blocks.end() ; )
                {
                    Block*block = *b;
                    
                    if( block->canBeRemoved() )
                    {
                        // Erase and point to the next one
                        b = blocks.erase(b);
                        
                        // Schedule remove file
                        engine::DiskOperation * operation = 
                        engine::DiskOperation::newRemoveOperation( block->getFileName() , getEngineId() );
                        engine::DiskManager::shared()->add( operation );
                        operation->release(); // It is now retained by disk manager
                        LM_T(LmtBlockManager,("In check, removing block:'%s'", block->str().c_str()));
                        
                        if( block->isWriting() || block->isReading() )
                            LM_X(1,("Not allowed to remove an object that is reading or writing..."));
                        
                        delete block;
                    }
                    else
                    {
                        //LM_T(LmtBlockManager,("In check, cannot remove block:'%s'", block->str().c_str()));
                        b++;
                    }
                }
            }
            
            // If no blocks, nothing to do...
            if ( blocks.size() == 0 )
                return;
            
            // --------------------------------------------------------------------------------
            // Find the blocks that should be in memory ( all until "limit_block" )
            // --------------------------------------------------------------------------------
            LM_T( LmtBlockManager , ("Detect limit_block"));
            size_t accumulated_memory = 0;
            Block * limit_block = *blocks.begin(); // Point to the first one
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
            {
                Block*block = *b;
                
                if( (accumulated_memory + block->getSize()) > max_memory )
                    break;
                
                accumulated_memory += block->getSize();
                limit_block = block;
            }
            
            if( !limit_block )
                LM_X(1,("Internal error"));
            
            LM_T(LmtBlockManager,("Block limit %s" , limit_block->str().c_str() ));

            // --------------------------------------------------------------------------------
            
            
            // --------------------------------------------------------------------------------
            // Free memory of blocks that are not suppouse to be on memory
            // --------------------------------------------------------------------------------
            {
                LM_T(LmtBlockManager,("Free blocks in memory under block_limit"));
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    // Considering this block
                    Block *block = *b;

                    // Stop when arrive to the limit block ( this should be in memory )
                    if( block == limit_block )
                        break;
                    
                    if( block->isReady() )  // Both on disk and on memory
                        if( !block->isLockedInMemory() )
                        {
                            LM_T(LmtBlockManager,("Free block:'%s'", block->str().c_str()));
                            
                            // Free block
                            block->freeBlock();
                        }
                }
            }
            
            // --------------------------------------------------------------------------------
            // Schedule write operations
            // --------------------------------------------------------------------------------
            
            if( scheduled_write_size < max_scheduled_write_size )
            {
                LM_T( LmtBlockManager , ("Schedule write operations"));
                // Lock for new write operations...
                std::list<Block*>::reverse_iterator b;
                for ( b = blocks.rbegin() ; b != blocks.rend() ; b++ )
                {
                    Block *block = *b;
                    
                    if( block->isOnMemory() )
                    {
                    	LM_T(LmtBlockManager,("Schedule write for block:'%s'", block->str().c_str()));
                        
                        // Schedule write
                        block->write();
                        
                        // Get size
                        size_t operation_size = block->getSize();
                        
                        scheduled_write_size += operation_size;
                        
                        LM_T(LmtBlockManager,("Write block of size %s" , au::str( operation_size ).c_str() ));
                        
                        // No continue for more writes
                        if ( scheduled_write_size >= max_scheduled_write_size )
                            break;
                    }
                }
            }
            
            // --------------------------------------------------------------------------------
            // Schedule read operations
            // --------------------------------------------------------------------------------
            
            // Schedule new reads operations ( high priority elements ) if available memory
            if( scheduled_read_size < max_scheduled_read_size )
            {
                LM_T( LmtBlockManager , ("Schedule read operations"));
                // Lock for new write operations...
                std::list<Block*>::iterator b;
                for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                {
                    Block *block = *b;

                    if( block->isOnDisk() ) // Needed to be loaded...
                    {
                    	LM_T(LmtBlockManager,("Trying to read block'%s'", block->str().c_str()));
                        
                        // Read the block
                        block->read();

                        LM_T(LmtBlockManager,("Scheduling read for block:'%s'", block->str().c_str()));

                        // Get size
                        size_t operation_size = block->getSize();
                        
                        scheduled_read_size += operation_size;
                        LM_T(LmtBlockManager,("Read block with size %s" , au::str(operation_size).c_str() ));
                        
                        // No continue for more writes
                        if( scheduled_read_size > max_scheduled_read_size )
                            break;
                    }
                    
                    // No schedule read operations over the block limit
                    if( block == limit_block )
                    {
                        LM_T(LmtBlockManager, ("Stops looking for read, because block_limit reached"));
                        break; // Not schedule reads on blocks that are not suppose to be on memory
                    }
                }
            }
            
        }

        void BlockManager::getInfo( std::ostringstream& output)
        {
            output << "<block_manager>\n";

            au::xml_simple( output , "scheduled_write_size" , scheduled_write_size );
            au::xml_simple( output , "scheduled_read_size"  , scheduled_read_size );

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
            // Sort the blocks to make sure it reflecs real order
            blocks.sort( compare_blocks );
            
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

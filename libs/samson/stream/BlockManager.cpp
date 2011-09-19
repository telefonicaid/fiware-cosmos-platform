#include <sys/types.h>
#include <dirent.h>

#include "engine/DiskManager.h"        // notification_disk_operation_request_response
#include "engine/Engine.h"             // engine::Engine
#include "engine/Notification.h"       // engine::Notification

#include "samson/common/SamsonSetup.h"          // samson::SamsonSetup
#include "samson/common/NotificationMessages.h" // notification_block_manager_review

#include "BlockList.h"          // BlockList
#include "BlockManager.h"       // Own interface


#define BLOCK_MANAGEMENT_MAX_WRITE_OPERATIONS   3
#define BLOCK_MANAGEMENT_MAX_READ_OPERATIONS    3

namespace samson {
    namespace stream {

        BlockManager *blockManager = NULL;
        
        BlockManager::BlockManager()
        {
            id = 1; // Start blocks by 1
 
            num_writing_operations=0;     // Number of writing operations ( low priority blocks )
            num_reading_operations=0;     // Number of reading operations ( high priority blocks )
        
            memory = 0;
            
            max_memory = SamsonSetup::getUInt64("general.memory")/2;

        }
        
        BlockManager::~BlockManager()
        {
        }

        void BlockManager::initOldFilesCheck()
        {
            listen( notification_block_manager_review );
            engine::Notification *notification = new engine::Notification(notification_block_manager_review );
            engine::Engine::shared()->notify( notification, 60 );   // Review every minute ( include in setup? )
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
            
            if ( notification->isName( notification_disk_operation_request_response ) )
            {
                
                std::string type = notification->environment.get("type", "-");
                
                //LM_M(("Received a notification with type %s" , type.c_str() ));
                
                if(  type == "write" )
                {
                    num_writing_operations--;
                    _review();
                }
                if( type == "read" )
                {
                    num_reading_operations--;
                    _review();
                }
                
            }
            else if ( notification->isName( notification_block_manager_review ) )
            {
                // Review old blocks to be removed...
                reviewOldFiles();
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
                        if( !block->isLockedInMemory() )
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
                    if( !block_to_be_free->isLockedInMemory() ) // Check that is not locked ( using in a process )
                    {
                        block_to_be_free->freeBlock();
                        memory -= block_to_be_free->size;

                        if( (memory + block->size ) < max_memory )
                            return;     // Enogth memory for the block we want to read
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
        
        
        void BlockManager::reviewOldFiles( )
        {
            // List of active blocks
            std::set<size_t> block_ids;
            
            std::list<Block*>::iterator it_block;
            for ( it_block =  blocks.begin() ; it_block != blocks.end() ; it_block++ )
                block_ids.insert( (*it_block)->getId() );

            
            // Get the list of files to be removed (  old blocks not used any more )
            std::set< std::string > remove_files;
            
            DIR *dp;
            struct dirent *dirp;
            if((dp  = opendir( SamsonSetup::shared()->blocksDirectory.c_str() )) == NULL) {
                
                // LOG and error to indicate that data directory cannot be access
                LM_W(("Not possible to open block directory %s to review old files" , SamsonSetup::shared()->blocksDirectory.c_str() ));
                return;
            }
            
            while ((dirp = readdir(dp)) != NULL) 
            {
                // Full path of the file
                std::string path = SamsonSetup::shared()->blocksDirectory + "/" + dirp->d_name;
                
                struct ::stat info;
                stat(path.c_str(), &info);
                
                if( S_ISREG(info.st_mode) )
                {
                    
                    // Get modification date to see if it was just created
                    time_t now;
                    time (&now);
                    double age = difftime ( now , info.st_mtime );
                    
                    if( age > 60*30 ) // Get some time (1 hour) to avoid sync errors
                    {
                        
                        // Get the task from the file name 
                        std::string file_name = dirp->d_name;
                        
                        size_t tmp_block_id = strtoll( file_name.c_str(), (char **)NULL, 10);

                        if( block_ids.find( tmp_block_id ) == block_ids.end() )
                        {
                            
                            // Add to be removed
                            remove_files.insert( path );
                        }
                    }
                }
            }
            closedir(dp);
            
            // Remove the selected files
            for ( std::set< std::string >::iterator f = remove_files.begin() ; f != remove_files.end() ; f++)
            {
                
                std::string fileName = *f;
                
                // Add a remove opertion to the engine ( target 0 means no specific listener to be notified )
                engine::DiskOperation * operation =  engine::DiskOperation::newRemoveOperation(  fileName , 0 );
                engine::DiskManager::shared()->add( operation );
            }
            
            
        }
        
        
    }
}

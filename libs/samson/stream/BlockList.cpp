
#include <sstream>       

#include "logMsg/logMsg.h"				// LM_M

#include "au/file.h"                                // au::sizeOfFile

#include "engine/ProcessManager.h"                  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/Info.h"                     // samson::Info
#include "samson/common/coding.h"                   // KVInfo

#include "samson/module/ModulesManager.h"           

#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/stream/QueueTasks.h"          // samson::stream::ParserQueueTask
#include "QueueTask.h"
#include "Queue.h"          // samson::stream::Queue
#include "StreamManager.h"  // samson::stream::StreamManager
#include "Block.h"          // samson::stream::Block
#include "BlockManager.h"   // samson::stream::BlockManager

#include "BlockList.h"                              // Own interface

namespace samson {
    namespace stream
    {
        
#pragma mark BlockList
        
        
        BlockList::~BlockList()
        {
            clearBlockList();
        }
        
        void BlockList::clearBlockList()
        {
            // Remove all the blocks contained in this list
            while( blocks.size() > 0 )
                remove( top() );
            
        }
        
        Block* BlockList::createBlock( engine::Buffer *buffer )
        {
            // create a new block
            Block *block  = new Block( buffer );

            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Add automatically to the Block Manager
            BlockManager::shared()->insert( block );   

            LM_T(LmtBlockManager, ("BlockList::createBlock created block: '%s'", block->str().c_str()));
         
            return block;
        }

        Block* BlockList::createBlockFromDisk( size_t id )
        {
            std::string fileName =  SamsonSetup::shared()->blockFileName( id );
            
            FILE *file = fopen( fileName.c_str() , "r" );
            if(!file)
                return NULL;
            
            KVHeader header;
            
            int r = fread( &header , sizeof(KVHeader) , 1 , file );
            if( r != 1 )
            {
                fclose( file );
                return NULL;
            }

            size_t fileSize = au::sizeOfFile( fileName );
            
            // Check file-size
            if ( !header.check_size( fileSize ) )
            {
                LM_W(("Not correct file while recovering block %lu from file %s" , id , fileName.c_str() ));
                fclose( file );
                return NULL;
            }
            
            
            // Add block as always
            Block *block = new Block( id , fileSize , &header );

            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Add automatically to the Block Manager
            BlockManager::shared()->insert( block );   
            
            fclose( file );
            
            LM_T(LmtBlockManager, ("BlockList::createBlock created block: '%s'", block->str().c_str()));

            return block;
        }
        
        
        void BlockList::add( Block *block )
        {
            
            // Accumulated information
            block->update( accumulated_block_info );
            
            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Check the block
            BlockManager::shared()->check( block );
            
            
        }
        
        void BlockList::remove( Block* block )
        {
            // Remove this block from my list of blocks
            blocks.remove( block );

            // remove this container from the list inside the block
            block->lists.erase( this );
         
            // Check the block
            BlockManager::shared()->check(block);
            
        }
        
        void BlockList::remove( BlockList* list )
        {
            au::list< Block >::iterator b;
            for (b=list->blocks.begin(); b != list->blocks.end();b++)
                remove(*b);
        }
        
        void BlockList::remove( size_t id )
        {
            au::list< Block >::iterator b;
            for (b=blocks.begin(); b != blocks.end();b++)
            {
                if( (*b)->id == id)
                {
                    remove( *b );
                    return;
                }
            }
        }

        
        Block* BlockList::top( )
        {
            if( blocks.size() == 0 )
                return NULL;
            return blocks.front();
        }
        
        size_t BlockList::getSize()
        {
            size_t total = 0;
            for (  au::list< Block >::iterator i = blocks.begin() ; i!= blocks.end() ; i++)
                total += (*i)->getSize();
            return total;
        }
        
        void BlockList::replace( BlockList *from , BlockList *to)
        {
            // Check all the blocks in from are included in this block list
            au::list< Block >::iterator b;
            for (b = from->blocks.begin() ;  b != from->blocks.end() ; b++)
            {
                size_t block_id = (*b)->getId();
                if( getBlock( block_id ) == NULL )
                    return; // not all the blocs are contained in this queue
            }
            
            // Remove all the blocks
            for (b = from->blocks.begin() ;  b != from->blocks.end() ; b++)
            {
                size_t block_id = (*b)->getId();
                remove( block_id );
            }
            
            // Insert all the blocks from the "to" list 
            copyFrom( to );
            
        }
        
        // Get a particular block ( only for debugging )
        Block* BlockList::getBlock( size_t _id )
        {
            for ( std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++ )
                if( (*i)->id == _id )
                    return *i;
            return NULL;
        }


        
        void BlockList::copyFrom( BlockList* list )
        {
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ;  b != list->blocks.end() ; b++)
                add( *b );
            
        }

        void BlockList::copyFrom( BlockList* list , KVRange range )
        {
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ;  b != list->blocks.end() ; b++)
                if( (*b)->isNecessaryForKVRange( range ) )
                    add( *b );
        }
        
        void BlockList::copyFrom( BlockList* list , size_t max_size )
        {
            copyFrom( list , KVRange( 0 , KVFILE_NUM_HASHGROUPS ) , false , max_size );
        }
        
        void BlockList::copyFrom( BlockList* list , KVRange range , bool exclusive , size_t max_size )
        {
            size_t total_size = 0 ;
            int num_blocks = 0;
            
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ;  b != list->blocks.end() ; b++)
            {
                Block *block = *b;
                KVRange block_range = block->getKVRange();
                
                if( range.overlap( block_range) )
                {
                    if( !exclusive || range.contains( block_range ) )
                    {
                        total_size += block->getSize();
                        if( num_blocks  > 0 )
                            if (( max_size > 0) && ( total_size > max_size ))
                                return;
                        
                        add( *b );
                    }
                }
            }
        }
        
        bool BlockList::extractBlockFrom( BlockList *list )
        {
            if( list->blocks.size() == 0)
            {
                return false;
            }
            
            Block* nextBlock = list->blocks.front();
            add( nextBlock );             // Add the block in this list
            list->remove( nextBlock );    // Remove from this list
            
            return true;
        }
        
        void BlockList::extractFrom( BlockList* list , size_t max_size )
        {
            size_t total_size = 0 ;
            int num_blocks = 0;
            
            Block* nextBlock = list->top();
            
            while( nextBlock )
            {
                // Increment the size
                total_size += nextBlock->getSize();

                if( num_blocks  > 0 )
                    if (( max_size > 0) && ( total_size > max_size ))
                    {
                    	LM_T(LmtBlockManager,("Stop extractFrom list '%s' at block %s with (total_size(%lu) > max_size(%lu)) after %d blocks", list->name.c_str(), nextBlock->str().c_str(), total_size, max_size, num_blocks));
                        return;
                    }
                
                add( nextBlock );
                list->remove( nextBlock );
                num_blocks++;
                
                // Get the next one
                nextBlock = list->top();
            }
            
        }

        
        bool BlockList::isEmpty()
        {
            return (blocks.size()==0);
        }
        
        
        
        bool BlockList::isContentOnMemory()
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                if( ! (*b)->isContentOnMemory() )
                {
                	LM_T(LmtBlockManager,("BlockList block id=%lu not in memory with %d requests", (*b)->getId(), (*b)->requests));
                	(*b)->requests += 25;
                	return false;
                }

            
            return true;
        }
        

        
        // Get information
        void BlockList::update( BlockInfo &block_info)
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                ( *b )->update( block_info );
        }

        size_t BlockList::getNumBlocks()
        {
            return blocks.size();
        }
        
        void BlockList::getInfo( std::ostringstream& output)
        {
            BlockInfo block_info;
            update( block_info );
            
            output << "<block_list>\n";
            block_info.getInfo( output );
            output << "</block_list>\n";
        }

        BlockInfo BlockList::getBlockInfo()
        {
            BlockInfo block_info;
            update( block_info);
            return  block_info;
        }
        
        std::string BlockList::strBlockIds()
        {
            std::ostringstream output;
            output << "[ ";
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                output << ( *b )->getId() << " ";
            output << "]";
            
            return output.str();
            
        }
        
        bool BlockList::isContained( KVRange range )
        {
            std::list<Block*>::iterator b;
            for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                if( !range.contains( (*b)->getKVRange() ) )
                    return false;
            return true;
        }
        
        bool BlockList::isAnyBlockIncludedIn( std::set<size_t> &block_ids)
        {
            // Check that no block is included in "block_ids_in_reduce_operations" 
            au::list< Block >::iterator block_it;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                size_t block_id = (*block_it)->getId();
                
                if( block_ids.find( block_id ) != block_ids.end() )
                    return true;    // At least one is included
            }
            return false;
        }
        
        void BlockList::removeBlockIdsAt( std::set<size_t> &block_ids )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                size_t block_id = (*block_it)->getId();
                block_ids.erase( block_id );
            }
        }
        
        void BlockList::addBlockIdsTo( std::set<size_t> &block_ids )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                size_t block_id = (*block_it)->getId();
                block_ids.insert( block_id );
            }
        }
        
        std::string BlockList::strRanges()
        {
            std::ostringstream output;
            
            au::list< Block >::iterator block_it;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                Block*block = (*block_it);
                output << au::str("<%lu %s>" , block->getId() , block->getKVRange().str().c_str() ) << "]";
            }
            
            
            return output.str();
        }
        
        std::string BlockList::strShortDescription()
        {
            BlockInfo block_info = getBlockInfo();
            return au::str("[ %d blocks | %s / %s | Mem %s Disk %s ]" , 
                           block_info.num_blocks , 
                           au::str(block_info.info.kvs,"kvs").c_str() , 
                           au::str(block_info.info.size,"B").c_str() , 
                           au::percentage_string( block_info.onMemoryPercentadge() ).c_str(),
                           au::percentage_string( block_info.onDiskPercentadge() ).c_str()
                           );
            
        }
        
        
    }       
}

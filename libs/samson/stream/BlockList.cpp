
#include <sstream>       

#include "logMsg/logMsg.h"				// LM_M

#include "au/file.h"                                // au::sizeOfFile

#include "engine/ProcessManager.h"                  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/coding.h"                   // KVInfo
#include "samson/common/SamsonSetup.h" 

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
            LM_T(LmtCleanup2, ("Removing all the blocks"));
            while( blocks.size() > 0 )
            {
                LM_T(LmtCleanup2, ("Removing a block: %d left",  blocks.size()));
                remove(top());
            }            
        }
        
        Block* BlockList::createBlock( engine::Buffer *buffer )
        {
            // Temporal list to contain the list of buffers
            engine::BufferListContainer buffer_list_container;
            buffer_list_container.push_back(buffer);
            
            return createBlock(&buffer_list_container);
        }
        
        
        Block* BlockList::createBlock( engine::BufferListContainer *buffer_list_container )
        {
            
            // Create a new block
            Block *block  = new Block( buffer_list_container );
            LM_T(LmtCleanup2, ("Creating a block"));

            LM_T(LmtBlockManager, ("BlockList::createBlock insert block: '%s'", block->str().c_str()));
            // Insert this block in my list
            blocks.push_back( block );
            
            LM_T(LmtBlockManager, ("BlockList::createBlock insert in lists block: '%s'", block->str().c_str()));
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            LM_T(LmtBlockManager, ("BlockList::createBlock insert in BlockManager block: '%s'", block->str().c_str()));
            // Add automatically to the Block Manager
            BlockManager::shared()->insert( block );   

            LM_T(LmtBlockManager, ("BlockList::createBlock created block: '%s'", block->str().c_str()));
         
            return block;
        }

        Block* BlockList::createBlockFromFile( std::string fileName )
        {
            //std::string fileName =  SamsonSetup::shared()->blockFileName( id );
            size_t worker_id;
            size_t id;
            
            if( !SamsonSetup::shared()->blockIdFromFileName( fileName ,&worker_id , &id ) )
            {
                LM_W(("Not possible to get ids for file %s to recover block" , fileName.c_str() ));
                return NULL;
            }
            
            FILE *file = fopen( fileName.c_str() , "r" );
            if(!file)
            {
                LM_W(("Not possible to open file %s to recover block" , fileName.c_str() ));
                return NULL;
            }
            
            KVHeader header;
            
            int r = fread( &header , sizeof(KVHeader) , 1 , file );
            if( r != 1 )
            {
                fclose( file );
                LM_W(("Not possible to read header for file %s" , fileName.c_str() ));
                return NULL;
            }

            size_t fileSize = au::sizeOfFile( fileName );
            
            // Check file-size
            if ( !header.check_size( fileSize ) )
            {
                LM_W(("Not correct size while recovering block %lu-%lu from file %s" , worker_id , id , fileName.c_str() ));
                fclose( file );
                return NULL;
            }
            
            
            // Add block as always
            Block *block = new Block( worker_id , id , fileSize , &header );
            LM_T(LmtCleanup2, ("Creating a block"));

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
            
        }
        
        void BlockList::remove( Block* block )
        {
            // Remove this block from my list of blocks
            blocks.remove( block );

            // remove this container from the list inside the block
            block->lists.erase( this );

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
        
        Block* BlockList::getNextBlockForDefrag()
        {
            Block* b = NULL;
            
            au::list< Block >::iterator it_blocks;
            for ( it_blocks = blocks.begin() ; it_blocks != blocks.end() ; it_blocks++) 
            {
                Block* b2 = *it_blocks;

                // Take it if no previous selection ( or minor minimum hash-group )
                if( !b || ( b->getKVRange().hg_begin > b2->getKVRange().hg_begin ) )
                    b = b2;
            }
            
            return b;
        
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
        
        void BlockList::copyFirstBlockFrom(BlockList* list, int hashgroup)
        {
            au::list<Block>::iterator  b;
            KVRange                    range(hashgroup, hashgroup + 1);

            for (b = list->blocks.begin(); b != list->blocks.end(); b++)
            {
                Block*  block       = *b;
                KVRange block_range = block->getKVRange();
                
                if (block_range.overlap(range))
                {
                    add(block);
                    return;
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

        void BlockList::extractFromForDefrag( BlockList* list , size_t max_size )
        {
            size_t total_size = 0 ;
            int num_blocks = 0;
            
            Block* nextBlock = list->getNextBlockForDefrag();
            
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
                
                // Get the next one...
                nextBlock = list->getNextBlockForDefrag();
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
                	LM_T(LmtBlockManager,("BlockList block id=%lu not in memory", (*b)->getId() ));
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
            std::ostringstream output;
            output << name;
            
            if( priority > 0 )
                output << " (p:" << priority << ")";

            if( task_id != (size_t) -1 )
                output << " (t:" << task_id << ")";
            
            return output.str();
        }
        
        double BlockList::getFragmentationFactor()
        {
            if( blocks.size() <= 1)
                return 0;
            
            int total_hgs = 0;
            int accumulated_num_blocks = 0;
            
            for ( int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++ )
            {
                int num_blocks = 0;
                std::list<Block*>::iterator b;
                for ( b = blocks.begin() ; b != blocks.end() ; b++ )
                {
                    Block* block = *b;
                    if ( block->header->range.contains(hg) )
                        num_blocks++;
                
                }
                
                if( num_blocks > 0 )
                {
                    accumulated_num_blocks += num_blocks;
                    total_hgs++;
                }
                    
            }
            
            double average_num_blocks = (double)accumulated_num_blocks / (double)total_hgs;
            return ( average_num_blocks - 1.0 ) / ( (double) ( blocks.size() - 1 ));
        }
        
        void BlockList::setPriority( int p )
        {
            priority = p;
            printf("Priority for list '%s' set to %d\n" , name.c_str() , priority );
        }

        void BlockList::setAsQueueBlockList()
        {
            queue = true;
        }
        
        size_t BlockList::getOldestBlockTime()
        {
            if( !queue )
                return -1;
            else if( blocks.size() == 0 )
                return -1;
            else
                return (blocks.front())->cronometer.diffTimeInSeconds();
            
        }

        bool BlockList::isBlockIncluded( Block* b )
        {
            au::list< Block >::iterator block_it;
            int pos = 0;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                Block*block = (*block_it);
                if( b == block )
                    return true;
                pos++;
            }
            return false;
        }

        size_t BlockList::getPosition(Block* b)
        {
            au::list< Block >::iterator block_it;
            int pos = 0;
            for ( block_it = blocks.begin() ; block_it != blocks.end() ; block_it++)
            {
                Block*block = (*block_it);
                if( b == block )
                    return pos;
                pos++;
            }
            return -1;
            
        }

        
    }       
}

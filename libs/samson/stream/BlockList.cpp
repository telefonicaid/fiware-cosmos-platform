
#include <sstream>       

#include "au/file.h"                                // au::sizeOfFile

#include "engine/ProcessManager.h"                  // engine::ProcessManager

#include "samson/common/EnvironmentOperations.h"    // getStatus()
#include "samson/common/Info.h"                     // samson::Info
#include "samson/common/coding.h"                   // KVInfo

#include "samson/module/ModulesManager.h"           

#include "samson/network/NetworkInterface.h"
#include "samson/worker/SamsonWorker.h"

#include "samson/stream/ParserQueueTask.h"          // samson::stream::ParserQueueTask
#include "QueueTask.h"
#include "Queue.h"          // samson::stream::Queue
#include "QueueItem.h"      // samson::stream::QueueItem
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
         
            return block;
        }

        Block* BlockList::createBlockFromDisk( size_t id )
        {
            std::string fileName =  Block::getFileNameForBlock( id );
            
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
            
            
            // Add block as allways
            Block *block = new Block( id , fileSize , &header );

            // Insert this block in my list
            blocks.push_back( block );
            
            // Insert myself in the the list inside the block
            block->lists.insert( this );
            
            // Add automatically to the Block Manager
            BlockManager::shared()->insert( block );   
            
            fclose( file );
            
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

        // get a block with this id ( if included in this list )
        Block* BlockList::getBlock( size_t id )
        {
            au::list< Block >::iterator b;
            for (b=blocks.begin(); b != blocks.end();b++)
                if( (*b)->id == id)
                    return *b;
            return NULL;
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
        
        /*
        void BlockList::copyFrom( BlockMatrix* matrix , int channel )
        {
            BlockList *bl = matrix->channels.findInMap( channel );
            if( !bl )
                return; // Nothing to copy because there is not such a channel
            
            // Copy all the blocks from another
            copyFrom( bl );
        }
         */
        
        
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
        
        // Copy all blocks from a queue that intersect with a range
        void BlockList::copyAllBlocksFrom( Queue* queue , KVRange range )
        {
            au::list< QueueItem >::iterator item_it , item_it1 , item_it2;
            for (item_it = queue->items.begin() ; item_it != queue->items.end() ; item_it++)
            {
                QueueItem *queueItem = *item_it;
                
                KVRange queue_item_range = queueItem->getKVRange();

                // copy content to this block list
                if( queue_item_range.overlap( range ) )
                    copyFrom( queueItem->list );
                
            }
            
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
                        return;
                
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
                    return false;
            
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

        
        
        
    }       
}

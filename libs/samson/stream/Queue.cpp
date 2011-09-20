

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup


#include "StreamManager.h"          // samson::stream::StreamManager
#include "BlockBreakQueueTask.h"    // samson::stream::BlockBreakQueueTask
#include "Block.h"                  // samson::stream::Block
#include "QueueItem.h"              // samson::stream::QueueItem
#include "BlockList.h"              // samson::stream::BlockList
#include "BlockBreak.h"             // samson::stream::BlockBreak
#include "Queue.h"                  // OwnInterface

namespace samson {
    namespace stream
    {
        
        Queue::Queue( std::string _name , StreamManager* _streamManager  )
        {
            // Keep the name
            name = _name;

            // Still pending of the block
            format = KVFormat("*","*");
            
            // Create a list for blocks pending to be broken...
            list = new BlockList( "queue_" + name );
            
            // Pointer to StreamManager
            streamManager = _streamManager;
            
            // By default it is not paused
            paused = false;
            
        }
        
        Queue::~Queue()
        {
            delete list;
        }

        
        void Queue::push( BlockList *list )
        {
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ; b != list->blocks.end() ; b++ )
                push( *b );
        }
        
        void Queue::push( Block *block )
        {
            
            if( format == KVFormat("*","*") )
                format = block->header->getKVFormat();
            else
            {
                if( format != block->header->getKVFormat() )
                {
                    LM_W(("Trying to push a block with format %s to queue %s with format %s. Block rejected", block->header->getKVFormat().str().c_str() , name.c_str() , format.str().c_str() ));
                    return;
                }
            }
            
            // Add to the list for this queue
            list->add( block );            
        }        
        
        void Queue::getInfo( std::ostringstream& output)
        {
            au::xml_open(output, "queue");

            au::xml_simple(output, "name", name );
            
            if( paused )
                au::xml_simple(output, "paused", "YES" );
            else
                au::xml_simple(output, "paused", "NO" );

            // Information about the format
            format.getInfo(output);

            // Block list information
            list->getInfo( output );
            
            // Information about content
            BlockInfo block_info;
            update( block_info );
            block_info.getInfo(output);
            
            au::xml_close(output, "queue");
        }
        
        void Queue::update( BlockInfo& block_info )
        {
            list->update( block_info );
            
        }
        
        void Queue::copyFrom( Queue* _queue )
        {
            list->copyFrom( _queue->list );
        }
        
        void Queue::copyTo( BlockList * _list , KVRange range )
        {
            au::list< Block >::iterator it_block;
            for (it_block = list->blocks.begin() ; it_block != list->blocks.end() ; it_block++ )
            {
                Block *block = *it_block;
                
                if ( block->getKVRange().overlap( range ) )
                {
                    
                    BlockList * auxBlockList = streamManager->getBlockBreak( block->getId() )->getMaxDivisionBlockList();
                    
                    if( auxBlockList )
                        _list->copyFrom( auxBlockList , range );
                    else
                    {
                        _list->add( block );
                    }
                }
            }
        }
        
        
        void Queue::setProperty( std::string property , std::string value )
        {
            environment.set( property , value );
        }
        
    }
}

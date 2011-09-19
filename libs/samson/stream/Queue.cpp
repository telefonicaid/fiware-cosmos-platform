

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup


#include "StreamManager.h"          // samson::stream::StreamManager
#include "BlockBreakQueueTask.h"    // samson::stream::BlockBreakQueueTask
#include "Block.h"                  // samson::stream::Block
#include "QueueItem.h"              // samson::stream::QueueItem
#include "BlockList.h"              // samson::stream::BlockList
#include "Queue.h"                  // OwnInterface

namespace samson {
    namespace stream
    {
        
        Queue::Queue( std::string _name , StreamManager* _streamManager ,  int num_items )
        {
            // Keep the name
            name = _name;

            // Still pending of the block
            format = KVFormat("*","*");
            
            // Create a list for blocks pending to be broken...
            pending = new BlockList( "pending_for_queue_" + name );
            
            // Pointer to StreamManager
            streamManager = _streamManager;
            
            // By default it is not paused
            paused = false;
            
            if( num_items <= 0)
                return;

            // Number of hash-groups per group
            int num_hash_groups_per_item = KVFILE_NUM_HASHGROUPS / num_items;
            
            // Create items for this queue
            for ( int i = 0 ; i < ( num_items - 1 ) ; i++)
            {                    
                KVRange range(i*num_hash_groups_per_item , (i+1)*num_hash_groups_per_item);
                QueueItem* item = new QueueItem( this  , range );
                items.push_back( item );
            }
            
            // The last one, overs the rest
            KVRange range((num_items-1)*num_hash_groups_per_item , KVFILE_NUM_HASHGROUPS );
            QueueItem* item = new QueueItem( this  , range );
            items.push_back( item );
            
            
        }
        
        Queue::~Queue()
        {
            // Remove all the item in the state
            items.clearList();
            
            delete pending;
            
        }

        void Queue::clearAndDivide( int num_divisions )
        {
            items.clearList();
            for (int i = 0 ; i < num_divisions ; i++)
            {
                QueueItem *item = new QueueItem( this , rangeForDivision(i, num_divisions) );
                items.push_back(item);
            }
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
            
            
            KVRange block_range = block->getKVRange();

            //LM_M(("Pushing block with range %d %d to queue %s" , block_range.hg_begin , block_range.hg_end , name.c_str() ));
            
            au::list< QueueItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
            {
                if( (*item)->getKVRange().includes( block_range ) )
                {
                    (*item)->push( block );
                    return;
                }
            }

            
            // Run a block-break operation for this block
            
            pending->add( block );  // Add temporary to this list ( removed when the operation ends )
            
            size_t id = streamManager->queueTaskManager.getNewId();
            
            BlockBreakQueueTask *tmp = new BlockBreakQueueTask( id , name ); 
            for (item = items.begin() ; item != items.end() ; item++ )
                tmp->addKVRange( (*item)->getKVRange() );
            
            tmp->getBlockList("input_0")->add( block );
            
            streamManager->queueTaskManager.add( tmp );

            
            
            
        }
        
        
        void Queue::getInfo( std::ostringstream& output)
        {
            au::xml_open(output, "queue");

            au::xml_simple(output, "name", name );
            au::xml_simple(output, "num_items", items.size() );
            
            if( paused )
                au::xml_simple(output, "paused", "YES" );
            else
                au::xml_simple(output, "paused", "NO" );

            format.getInfo(output);
            
            // Iterate to all the items
            au::xml_iterate_list( output , "items" , items );
            
            // Information about content
            BlockInfo block_info;
            update( block_info );
            block_info.getInfo(output);
            
            au::xml_close(output, "queue");
        }
        
        void Queue::update( BlockInfo& block_info )
        {
            
            au::list< QueueItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                (*item)->update( block_info );

            
            pending->update( block_info );
            
        }
        
        void Queue::divide( QueueItem *item , QueueItem *item1 , QueueItem *item2 )
        {
            // Divide this item in two items
            au::list< QueueItem >::iterator item_it , item_it1 , item_it2;
            
            for (item_it = items.begin() ; item_it != items.end() ; item_it++)
            {
                if( (*item_it) == item)
                {
                    
                    item_it1 = items.erase( item_it );
                    
                    item_it2 = items.insert(item_it1, item1);
                    items.insert(item_it2, item1);
                    
                    // Delete the old item
                    delete item;
                    
                    return;
                    
                }
            }
            
        }

        bool Queue::isWorking()
        {
            au::list< QueueItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                if( (*item)->isWorking() )
                    return true;
            return false;
        }
        
        void Queue::copyFrom( Queue* _queue)
        {
            // Copy all the blocks from one queue to another
            au::list< QueueItem >::iterator item;
            for (item = _queue->items.begin() ; item != _queue->items.end() ; item++ )
                push ( (*item)->list );
            
        }
        
        void Queue::setProperty( std::string property , std::string value )
        {
            
            if( property == "system.divisions" )
            {
                clearAndDivide( atoi( value.c_str() ) );
            }
            else
                environment.set( property , value );
            
        }
      

        
    }
}

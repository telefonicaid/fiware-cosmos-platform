

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup

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
        
        void Queue::distributeItemsAs( Queue* otherQueue )
        {
            if( isDistributedAs( otherQueue ) )
               return;  // already distributed in the same way


            au::list< QueueItem > future_items;

            au::list< QueueItem >::iterator qi;
            for ( qi = otherQueue->items.begin() ; qi != otherQueue->items.end() ; qi++ )
            {
                QueueItem *item = new QueueItem( this , (*qi)->range );

                //Push data into this new item
                au::list< QueueItem >::iterator qi_previous;
                for ( qi_previous = items.begin() ; qi != items.end() ; qi++ )
                    if ( (*qi_previous)->range.overlap( item->range ) )
                        item->push( (*qi_previous)->list );
                
                // Save it locally
                future_items.push_back( item );    
            }
            
            // Remove current items
            items.clearList();
            
            // Insert new ones
            for ( qi = future_items.begin() ; qi != future_items.end() ; qi++ )
                items.push_back(*qi);
            
            
            if( !isDistributedAs( otherQueue ) )
                LM_X(1,("Internal error. not possible to distribute queue in the same way"));
            
        }

        bool Queue::isDistributedAs( Queue* otherQueue )
        {
            if( items.size() != otherQueue->items.size() )
                return false;

            au::list< QueueItem >::iterator qi1,qi2;
            for ( qi1 = items.begin() , qi2 = otherQueue->items.begin() ; qi1 != items.end() ; qi1++ , qi2++ )
                if( (*qi1)->range != (*qi2)->range )
                    return false;
            
            return true;
        }

        
        Queue::~Queue()
        {
            // Remove all the item in the state
            items.clearList();
        }
        
        void Queue::push( BlockList *list )
        {
            au::list< QueueItem >::iterator item;
            for (item = items.begin() ; item != items.end() ; item++ )
                (*item)->push( list );
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
            {
                (*item)->update( block_info );
            }
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
        
       
    }
}

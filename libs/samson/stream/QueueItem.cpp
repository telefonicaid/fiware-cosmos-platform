

#include "Queue.h"      // samson::stream::Queue
#include "Block.h"      // samson::stream::Block
#include "BlockList.h"  // samson::stream::BlockList
#include "QueueTask.h"      // samson::stream::QueueTask

#include "QueueItem.h"  // Own interface

namespace samson {

namespace stream
    {
        
        QueueItem::QueueItem( Queue * _myQueue,  KVRange _range )
        {

            // Keep reference to the queue "I" belong
            myQueue = _myQueue;

            // Limits of application
            range = _range;
            
            // Cleate the list of blocks properly
            list = new BlockList( _myQueue->name + "_item" );
            future_list = new BlockList( _myQueue->name + "_item_future" );

            // By default I am not working
            working = false;
            task = NULL;

        }
        
        QueueItem::~QueueItem()
        {
            delete list;
            delete future_list;
        }
     
        void QueueItem::push( BlockList *_list )
        {
            if (working )
            {
                LM_W(("Not possible to push data to a working queue"));
                return;
            }
            
            prepareBlockList(_list);            
            list->copyFrom(_list , range );
        }
        
        void QueueItem::push( QueueTask * _task , engine::Buffer *buffer )
        {
            if ( _task == task )
            {
                Block* b = future_list->createBlock(buffer, false);
                prepareBlock(b);
            }
            else
            {
                LM_W(("Error trying to add a buffer to state-queue that is working for another task"));
            }
        }

        void QueueItem::getInfo( std::ostringstream& output)
        {
            au::xml_open(output , "queue_item");
            
            range.getInfo(output);
            
            au::xml_simple( output , "working", working);

            // Information about content
            getFullKVInfo().getInfo(output);
            
            output << "<list>";
            list->getInfo(output);
            output << "</list>\n";

            output << "<future_list>";
            future_list->getInfo(output);
            output << "</future_list>\n";
            
            
            au::xml_close(output , "queue_item");
        }

        void QueueItem::setRunning( QueueTask* _task )
        {
            // Set the flag to working
            working = true;

            // Point to the current task
            task = _task;
            
            // Just in case, we make sure the future list is empty
            future_list->clearBlockList();
        }

        
        void QueueItem::notifyFinishOperation( QueueTask* _task )
        {
            if( !working )
                LM_X(1, ("Internal error"));
            
            if( task != _task)
                LM_X(1,("Internal error"));
            
            // Use the newly generated list of blocks
            list->clearBlockList();
            list->copyFrom(future_list);
            future_list->clearBlockList();
            
            // We are not working any more
            working = false;
            task = NULL;
        }
        
        bool QueueItem::isWorking()
        {
            return working;
        }
        
        void QueueItem::prepareBlockList( BlockList *list )
        {
            au::list< Block >::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++ )
                prepareBlock(*b);
        }
        
        void QueueItem::prepareBlock( Block *block )
        {
            block->computeKVInfoForRange( range );
            block->computeKVInfoForRange( range.firstHalf() );
            block->computeKVInfoForRange( range.secondHalf() );
            
            block->computeKVInfoForRange( range.firstQuarter() );
            block->computeKVInfoForRange( range.secondQuarter() );
            block->computeKVInfoForRange( range.thirdQuarter() );
            block->computeKVInfoForRange( range.fourthQuarter() );
            
        }
        
        FullKVInfo QueueItem::getFullKVInfo()
        {
            return list->getFullKVInfo( range );
        }

        
    }
}
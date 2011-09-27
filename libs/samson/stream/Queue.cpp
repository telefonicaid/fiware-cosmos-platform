

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup


#include "StreamManager.h"          // samson::stream::StreamManager
#include "BlockBreakQueueTask.h"    // samson::stream::BlockBreakQueueTask
#include "Block.h"                  // samson::stream::Block
#include "QueueItem.h"              // samson::stream::QueueItem
#include "BlockList.h"              // samson::stream::BlockList
#include "BlockBreak.h"             // samson::stream::BlockBreak
#include "Queue.h"                  // OwnInterface


size_t next_pow_2( size_t value )
{
    if( value < 2)
        return 1;

    int p = 1;
    while ( true )
    {
	   if( value == pow(2.0 , p)) 
           return pow( 2.0 , p);
        
        if( value < pow( 2.0 , p ) )
            return pow( 2.0 , p );
        p++;
    }
    
    LM_X(1,("Internal error"));
    return 1;
    
}

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
            
            // Number of divisions for this queue
            num_divisions = 1;
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

            au::xml_simple(output, "num_divisions", num_divisions);
            
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
        
        void Queue::setProperty( std::string property , std::string value )
        {
            environment.set( property , value );
        }
        
        void Queue::review()
        {
            // Get the info for this block
            BlockInfo block_info;
            update( block_info );
         
            
            // Compute division necesary for this block    
            double _min_num_divisions = 4.0 * (double)block_info.size / (double) SamsonSetup::shared()->getUInt64("general.memory");
            int min_num_divisions = next_pow_2( (size_t) _min_num_divisions ); 

            // Set the new value for num_divisions if necessary
            if( num_divisions < min_num_divisions )
                num_divisions = min_num_divisions;

            
            // Schedule new Block Break operations if necessary
            if( format.isTxt() )
                return;  // No necessry operations for txt elements

            
            if( num_divisions > 1 )
            {
                
                LM_M(("Queue %s: Num divisions %d" , name.c_str() , num_divisions));
                
                BlockList *tmp = new BlockList("candidates_block_division");
                
                au::list< Block >::iterator block_it;
                for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++)
                {
                    Block* block = *block_it;
                    KVRange range = block->getKVRange();
                    
                    
                    if( !range.isOkForNumDivisions( num_divisions ) )
                    {
                        size_t block_id = block->getId();
                        
                        if( block_ids_in_break_operations.find( block_id ) == block_ids_in_break_operations.end() )
                        {
                            LM_M(("Considered block %lu ( %s ) for breaking..." , block->getId() , block->getKVRange().str().c_str() ));
                            tmp->add( block );
                        }
                    }
                }
                
                size_t min_size_block_break_task = (double)  (double) SamsonSetup::shared()->getUInt64("general.memory") /  (double) SamsonSetup::shared()->getUInt64("general.num_processess");
                
                // Run break operations as necessary
                while( tmp->getBlockInfo().size > min_size_block_break_task )
                {
                    // Schedule a block break operation
                    size_t id = streamManager->queueTaskManager.getNewId();
                    
                    BlockBreakQueueTask *task = new BlockBreakQueueTask( id , name , num_divisions ); 
                    BlockList *input = task->getBlockList("input_0");
                    input->extractFrom( tmp , min_size_block_break_task ); 
                    
                    // add to "block_ids_in_break_operations"
                    // Remove ids from block_ids_in_break_operations
                    au::list< Block >::iterator block_it;
                    for ( block_it = input->blocks.begin() ; block_it != input->blocks.end() ; block_it++)
                    {
                        size_t block_id = (*block_it)->getId();
                        block_ids_in_break_operations.insert( block_id );
                    }

                    streamManager->queueTaskManager.add( task );
                    LM_M(("Running a block-break operation for queue %s %s" , name.c_str() , input->strBlockIds().c_str() ));                     
                }
                
                
                size_t tmp_size = tmp->getBlockInfo().size;
                if( tmp_size >0 )
                    LM_M(("Not running any operation since pending size if %lu ( min %lu ) " , tmp_size , min_size_block_break_task ));
                
                
                delete tmp;
            }
            
            
            // Compact if the over-heade is too high for a particular division
            if ( num_divisions > 1 )
            {
                for ( int n = 0 ; n < num_divisions ; n++)
                {
                    // Create a list with all the blocks exclusive for this division
                    BlockList *tmp = new BlockList("compactation_for_queue");
                    
                    KVRange range = rangeForDivision(n, num_divisions);
                    au::list< Block >::iterator block_it;
                    for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++)
                    {
                        Block *block = *block_it;
                        if ( range.includes( block->getKVRange() ) )
                            tmp->add( block );
                    }

                    
                    // If overhead is too large, compactation is required
                    BlockInfo block_info = tmp->getBlockInfo();
                        
                    LM_M(("Queue %s: division %d/%d %d blocks with %f overhead", name.c_str() , n , num_divisions , block_info.num_blocks , block_info.getOverhead() ));

                    if ( block_info.getOverhead() > 0.2 )
                    {
                        // Run a compact operation over this set
                        // Future work...
                    }
                    
                    delete tmp;
                    
                }
                
            }
            
            
        }
        void Queue::notifyFinishBlockBreakQueueTask( BlockList *from , BlockList *to )
        {
            LM_M(("Finish a block-break operation for queue %s" , name.c_str() )); 
            LM_M(("FROM: %s" , from->strBlockIds().c_str() ));
            LM_M(("TO: %s" , to->strBlockIds().c_str() ));
            
            // replace the original blocks for the new ones
            list->replace( from , to );
            
            // Remove ids from block_ids_in_break_operations
            au::list< Block >::iterator block_it;
            for ( block_it = from->blocks.begin() ; block_it != from->blocks.end() ; block_it++)
            {
                size_t block_id = (*block_it)->getId();
                block_ids_in_break_operations.erase( block_id );
            }
            
            
            
        }

        
    }
}

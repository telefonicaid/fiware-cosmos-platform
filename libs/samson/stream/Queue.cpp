

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup


#include "StreamManager.h"          // samson::stream::StreamManager
#include "BlockBreakQueueTask.h"    // samson::stream::BlockBreakQueueTask
#include "Block.h"                  // samson::stream::Block
#include "BlockList.h"              // samson::stream::BlockList
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
        
        
        
        void BlockIdList::addIds( BlockList *list )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++)
            {
                Block* block = *block_it;
                size_t block_id = block->getId();
                addId( block_id );
            }
            
        }
        void BlockIdList::removeIds( BlockList *list )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++)
            {
                Block* block = *block_it;
                size_t block_id = block->getId();
                removeId( block_id );
            }
        }
        
        void BlockIdList::addId( size_t id )
        {
            block_ids.insert( id );
        }
        void BlockIdList::removeId( size_t id )
        {
            block_ids.erase( id );
        }
        
        size_t BlockIdList::num_ids()
        {
            return block_ids.size();
        }
        
        bool BlockIdList::containsBlockId( size_t id )
        {
            return ( block_ids.find( id ) != block_ids.end() );
        }
        
        
#pragma mark Queue
        
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
            
            // Default number of divisions
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
                format = block->header->getKVFormat();  // Get the format of the first one...
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

            // Information about the format
            format.getInfo(output);

            // Block list information
            list->getInfo( output );
            
            // Informat description of queue status
            std::ostringstream status;
            
            au::xml_simple(output, "num_divisions", num_divisions );
                        
            if( lock_block_ids.num_ids() > 0)
                status << au::str("[ Locked %d/%d blocks ] " , lock_block_ids.num_ids() , list->getNumBlocks()); 

            if( status.str() == "" )
                status << "ready";
                                 
            au::xml_simple( output , "status" , status.str() );

            // Information about content
            BlockInfo block_info;
            update( block_info );
            block_info.getInfo(output);

            
            au::xml_simple( output , "enviromemnt" , environment.getEnvironmentDescription() );
            
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
        
        void Queue::replaceAndUnlock( BlockList *from , BlockList *to )
        {
            replace( from , to );
            unlock(from);
        }
        
        void Queue::replace( BlockList *from , BlockList *to )
        {
            // Replace a list of blocks by another list
            list->replace( from , to );
        }

        void Queue::removeAndUnlock( BlockList *_list )
        {
            list->remove( _list );
            unlock( _list );
        }
        
        void Queue::remove ( BlockList *_list )
        {
            list->remove( _list );
        }
        
        void Queue::unlock ( BlockList *list )
        {
            // Remove this ids from any possible concept
            lock_block_ids.removeIds( list );
        }
        
        bool Queue::lock ( BlockList *list )
        {
            if( !canBelock( list ) )
                return false;

            // Add the block sto the id
            lock_block_ids.addIds( list );
            return true;
        }
        
        bool Queue::canBelock ( BlockList *list )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block* block = *block_it;
                if( lock_block_ids.containsBlockId( block->getId() ) )
                    return false;
            }
            
            return true;
        }

        bool Queue::getAndLockBlocksForKVRange( KVRange range , BlockList *outputBlockList )
        {
            if( !canLockBlocksForKVRange(range) )
                return false;
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                size_t block_id = block->getId();
                KVRange block_range = block->getKVRange();
                
                if( block_range.overlap( range ) )
                {
                    lock_block_ids.addId( block_id );
                    outputBlockList->add( block );
                }
            }
            return true;
        }
        
        bool Queue::canLockBlocksForKVRange( KVRange range )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                size_t block_id = block->getId();
                KVRange block_range = block->getKVRange();
                
                if( block_range.overlap( range ) )
                    if( lock_block_ids.containsBlockId(block_id) )
                        return false;
                
            }
            return true;
        }
        

        void Queue::review()
        {
            
            // Review the size contained in this queue if there is a limit defined....
            
            size_t max_size = environment.getSizeT( "max_size" , 0 );
            
            if( max_size > 0 )
            {

                BlockList tmp_block_list;   // Temporal list to put extracted blocks...
                while( list->getBlockInfo().size > max_size )
                    tmp_block_list.extractBlockFrom( list );
            }
            
            //LM_M(("Intern review queue %s" , name.c_str() ));
            
            // Schedule new Block Break operations if necessary
            if( format == KVFormat("txt","txt") )
			{
                //LM_M(("Queue %s nor revied since format %s= txt" , name.c_str() , format.str().c_str() ));
                return;  // No necessry operations for txt elements
			}
            
            // Set the minimum number of divisions ( when possible )
            // Only in state queues when trying to run update-state operations
            // setMinimumNumDivisions();
            
            while( true )
            {
                //LM_M(("Queue %s: Num divisions %d.... %lu blocks" , name.c_str() , num_divisions , list->blocks.size() ));
                
                size_t max_size = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");
                
                BlockList inputBlockList("candidates_block_division");
                getBlocksToBreak( &inputBlockList , max_size );

                if( inputBlockList.isEmpty() )
                    break;
                
                // Schedule a block break operation
                size_t id = streamManager->queueTaskManager.getNewId();
                
                BlockBreakQueueTask *task = new BlockBreakQueueTask( id , name , num_divisions ); 
                BlockList *input = task->getBlockList("input_0");
                input->copyFrom( &inputBlockList , 0 ); 
                
                task->setWorkingSize();
                
                streamManager->queueTaskManager.add( task );
                
                //LM_M(("Running a block-break operation for queue %s %s" , name.c_str() , input->strBlockIds().c_str() ));                     
            }
            
            /*
            // Compact if the over-headed is too high for a particular division
            //LM_TODO(("To be completed..."));
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
                    
                    //LM_M(("Queue %s: division %d/%d %d blocks with %f overhead", name.c_str() , n , num_divisions , block_info.num_blocks , block_info.getOverhead() ));
                    
                    if ( block_info.getOverhead() > 0.2 )
                    {
                        // Run a compact operation over this set
                        // Future work...
                    }
                    
                    delete tmp;
                    
                }
                
            }
             */
            
        }

        // Get some blocks that need to be broken to meet a particular number of divisions ( not locked )
        void Queue::getBlocksToBreak( BlockList *outputBlockList  , size_t max_size )
        {
            
            size_t total_size = 0 ;
            int num_blocks = 0;
            
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ;  b != list->blocks.end() ; b++)
            {
                Block *block = *b;
                size_t block_id = block->getId();
                KVRange block_range = block->getKVRange();
                
                
                if( !lock_block_ids.containsBlockId( block_id ) )
                {
                    if( !block_range.isValidForNumDivisions( num_divisions ) )
                    {
                        total_size += block->getSize();
                        if( num_blocks  > 0 )
                            if (( max_size > 0) && ( total_size > max_size ))
                                return;
                        
                        lock_block_ids.addId( block->getId() );
                        
                        outputBlockList->add( *b );
                    }
                    
                }
            }            
        }
        
        bool Queue::isReadyForDivisions( )
        {
            // Check the queue is ready for the number of divisions ( all the block break operations have finished )
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                KVRange block_range = block->getKVRange();
                
                if( !block_range.isValidForNumDivisions( num_divisions ) )
                    return false;
            }
            
            return true;
            
        }
        
        
    }
}

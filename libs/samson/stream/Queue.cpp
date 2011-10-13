

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
            
            // By default it is not paused
            paused = false;
            
            // Number of divisions for this queue
            num_divisions = 1;
            
            // Number of updates ( 0 at start )
            updates = 0;
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
            
            // Review this queue
            review();
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
            
            // Informat description of queue status
            std::ostringstream status;
            
            if( updating_divisions.size() > 0)
            {
                status << au::str("[ Updating state %d / %d ] " , updating_divisions.size() , num_divisions );
                
                if( getMinNumDivisions() > num_divisions )
                {
                    status << au::str("[ Moving from %d to %d divisions ] " , num_divisions , getMinNumDivisions() );
                }
                
                if( breaking_block_ids.num_ids() > 0)
                    status << "[ Error: breaking tasks while updating state ] ";
            }
            
            
            if( breaking_block_ids.num_ids() > 0)
                status << au::str("[ Breaking %d/%d blocks ] " , breaking_block_ids.num_ids() , list->getNumBlocks()); 

            if( processing_block_ids.num_ids() > 0)
                status << au::str("[ Processing %d/%d blocks ] " , processing_block_ids.num_ids() , list->getNumBlocks()); 
            
            if( status.str() == "" )
                status << "ready";
                                 
            au::xml_simple( output , "status" , status.str() );

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
        
        void Queue::setNumDivisions( int new_num_divisions )
        {
            // It is possible to change the number of divisions only when there are not update-state operations
            if( new_num_divisions > num_divisions )
                if( updating_divisions.size() == 0)
                    num_divisions = new_num_divisions;
        }
        
        int Queue::getMinNumDivisions()
        {            
            BlockInfo block_info;
            update( block_info );

            double _min_num_divisions = (double)block_info.size / (double) SamsonSetup::shared()->getUInt64("stream.max_state_division_size");
            int min_num_divisions = next_pow_2( (size_t) _min_num_divisions ); 

            int num_divisions_base = SamsonSetup::shared()->getInt("general.num_processess");
            
            // Minimum the number of cores and then 4*num_cores...
            while( num_divisions_base < min_num_divisions )
                num_divisions_base *= 4;
                
            return num_divisions_base;
        }
        
        void Queue::setMinimumNumDivisions()
        {
            // Set this number if possible
            setNumDivisions( getMinNumDivisions() );
        }
        
        void Queue::review()
        {
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
            
            // No block-break operations if there are current update-state operations
            if( updating_divisions.size() > 0)
            {
                //LM_M(("Queue %s: Non cheking since there are %d updating divisions" , name.c_str() , (int) updating_divisions.size() ));
                return;
            }
            
            if( num_divisions == 1 )
            {
			   //LM_M(("Queue %s nor revied since num_division = 1" , name.c_str() ));
            }
			else
            {
                
                //LM_M(("Queue %s: Num divisions %d.... %lu blocks" , name.c_str() , num_divisions , list->blocks.size() ));
                
                BlockList *tmp = new BlockList("candidates_block_division");
                
                au::list< Block >::iterator block_it;
                for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++)
                {
                    Block* block = *block_it;
                    //size_t block_id = block->getId() ;
                    KVRange range = block->getKVRange();
                    
                    //LM_M(("Considered block %lu ( %s ) for breaking..." , block_id, block->getKVRange().str().c_str() ));
                    
                    if( !range.isValidForNumDivisions( num_divisions ) )
                    {
                        size_t block_id = block->getId();
                        
                        if( ! isBlockIdLocked( block_id ) ) // Only consider blocks that are not locked for any operation
                        {
                            //LM_M(("Considered block %lu ( %s ) for breaking..." , block->getId() , block->getKVRange().str().c_str() ));
                            tmp->add( block );
                        }
                        else
                        {
                            //LM_M(("Block %lu is blocked" , block_id , num_divisions ));
                        }
                    }
                    else
                    {
                        //LM_M(("Block %lu is ok for %d divisions" , block_id , num_divisions ));
                    }
                }
                
                size_t max_size = SamsonSetup::shared()->getUInt64("stream.max_operation_input_size");
                
                // Run break operations as necessary
                while( tmp->getNumBlocks() > 0 )    // While there is something to break...
                {
                    // Schedule a block break operation
                    size_t id = streamManager->queueTaskManager.getNewId();
                    
                    BlockBreakQueueTask *task = new BlockBreakQueueTask( id , name , num_divisions ); 
                    BlockList *input = task->getBlockList("input_0");
                    input->extractFrom( tmp , max_size ); 
                    
                    task->setWorkingSize();
                    
                    // add to "block_ids_in_break_operations"
                    // Remove ids from block_ids_in_break_operations
                    au::list< Block >::iterator block_it;
                    for ( block_it = input->blocks.begin() ; block_it != input->blocks.end() ; block_it++)
                    {
                        size_t block_id = (*block_it)->getId();
                        breaking_block_ids.addId(block_id);
                    }

                    streamManager->queueTaskManager.add( task );
                    //LM_M(("Running a block-break operation for queue %s %s" , name.c_str() , input->strBlockIds().c_str() ));                     
                }
                
                
                delete tmp;
            }
            
            
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
            breaking_block_ids.removeIds( list );
            processing_block_ids.removeIds( list );
        }

        
        
        bool Queue::isQueueReadyForStateUpdate()
        {
            // If a queue is paused, it is not ready to run any update-state operation
            if( paused )
                return false;
            
            // If we need more divisions for this queue, no more update operations here
            if( getMinNumDivisions() > num_divisions )
                return false;

            // Check all the blocks are perfectly divided acording to the num_divisions
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
        
        
#pragma mark UPDATE STATE ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
        
        bool Queue::lockDivision( int division )
        {
            if( updating_divisions.find( division ) == updating_divisions.end() )
            {
                updating_divisions.insert( division );
                return true;
            }
            else
                return false;   // Already blocked
        }
        
        bool Queue::lockAllDivisions()
        {
            if( updating_divisions.size() != 0)
                return false;

            // Lock all divisions
            for (int i = 0 ; i < num_divisions ; i++ )
                updating_divisions.insert( i );
            return true;
            
        }
        
        void Queue::unlockDivision( int division )
        {
            updating_divisions.erase( division );
        }
        
        void Queue::getStateBlocksForDivision( int division , BlockList *outputBlockList )
        {
            KVRange range = rangeForDivision( division , num_divisions );
            
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                KVRange block_range = block->getKVRange();
                
                if( block_range.overlap( range ) )
                    outputBlockList->add( block );
            }
        }
        
        BlockList *Queue::getInputBlockListForRange( KVRange range , size_t max_size )
        {
            int num_blocks = 0;
            size_t total_size = 0;
            
            BlockList *tmp = new BlockList( "getInputBlockListForRange" );
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                KVRange block_range = block->getKVRange();
                
                if( block_range.overlap( range ) )
                {
                    if( range.contains( block_range ) )
                    {
                        total_size+=block->size;
                        
                        if( num_blocks > 0 )
                            if( ( max_size > 0 ) && ( total_size > max_size) )
                                return tmp;
                        
                        tmp->add( block );
                    }
                }
            }
            
            return tmp;
        }
        
        BlockList *Queue::getInputBlockListForProcessing( size_t max_size )
        {
            int num_blocks = 0;
            size_t total_size = 0;
            
            BlockList *tmp = new BlockList( "getInputBlockListForProcessing" );
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                size_t block_id = block->getId();
                
                if( !isBlockIdLocked(block_id) )
                {
                    
                    total_size+=block->size;
                    
                    if( num_blocks > 0 )
                        if( ( max_size > 0 ) && ( total_size > max_size) )
                            return tmp;

                    num_blocks++;                    
                    tmp->add( block );
                }
            }
            
            // Add to the list of currently blocked ids for processing
            processing_block_ids.addIds( tmp );
            
            return tmp;
        }        

        BlockList *Queue::getInputBlockListForProcessing( size_t max_size , BlockIdList* used_blocks )
        {
            int num_blocks = 0;
            size_t total_size = 0;
            
            BlockList *tmp = new BlockList( "getInputBlockListForProcessing" );
            
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                size_t block_id = block->getId();
                
                if( !isBlockIdLocked(block_id) && !used_blocks->containsBlockId( block_id ) )
                {
                    
                    total_size+=block->size;

                    if( num_blocks > 0 )
                        if( ( max_size > 0 ) && ( total_size > max_size) )
                            return tmp;

                    num_blocks++;
                    tmp->add( block );
                    used_blocks->addId( block->getId() );   // Add to this list to not use again in the next call to this function
                }
            }
            
            return tmp;
        }        
        
        bool Queue::isBlockIdLocked( size_t id )
        {
            if( breaking_block_ids.containsBlockId(id) )
                return true;
            if( processing_block_ids.containsBlockId(id) )
                return true;
            
            return false;
        }
        
        
    }
}

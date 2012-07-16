
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/coding.h"           // KVFILE_NUM_HASHGROUPS
#include "samson/common/MessagesOperations.h"
#include "samson/common/SamsonSetup.h"      // samson::SamsonSetup

#include "samson/stream/StreamOperation.h"
#include "samson/stream/StreamOutput.h"

#include "StreamManager.h"          // samson::stream::StreamManager
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
        
        
        void BufferAccumulator::flush()
        {
            
            while( buffer_list_container.getNumBuffers() > 0 )
            {
                // Extract available buffers ( at least one ) with a maximum size of 64Mb
                engine::BufferListContainer tmp_buffer_list_container;
                tmp_buffer_list_container.extractFrom( &buffer_list_container , 64000000 );
                
                // Tmp list to hold the new block
                BlockList tmp_block_list;
                
                /*
                if( tmp_buffer_list_container.getNumBuffers() > 1 )
                    LM_W(("Creating block from multiple buffers %s" , queue->name.c_str() ));
                */
                 
                // Create a block out of this buffers
                Block* block = tmp_block_list.createBlock( &tmp_buffer_list_container );

                // Push the new block into the queue
                queue->push( block );
            }
            
        }
        
        size_t BufferAccumulator::getTotalSize()
        {
            return buffer_list_container.getTotalSize();
        }

        
#pragma mark Queue
        
        Queue::Queue( std::string _name , StreamManager* _streamManager  ) : buffer_accumulator(this)
        {
            // Keep the name
            name = _name;

            // Still pending of the block
            format = KVFormat("*","*");
            
            // Create a list for blocks pending to be broken...
            list = new BlockList( "queue_" + name );
            list->setAsQueueBlockList();
            
            // Pointer to StreamManager
            streamManager = _streamManager;
            
            // Default number of divisions
            num_divisions = 1;
            
        }
        
        Queue::~Queue()
        {
            delete list;
        }
        
        void Queue::push( engine::Buffer * buffer )
        {
            // Accumulate new buffers in this auto-retained list of buffers
            buffer_accumulator.push(buffer);
        }

        void Queue::flushBuffers()
        {
            buffer_accumulator.flush();
        }
        
        void Queue::push( Block *block )
        {
            // Temporal block list used in some interfaces with stream operations
            BlockList block_list;
            block_list.add( block );
            
            
            if( format == KVFormat("*","*") )
                format = block->header->getKVFormat();  // Get the format of the first one...
            else
            {
                if( format != block->header->getKVFormat() )
                {
                    std::string message 
                    = au::str("Rejecting %s block. Not inserted in queue %s ( Wrong format  %s != %s )"
                              , au::str( block->size , "B").c_str()
                              , name.c_str() 
                              , block->header->getKVFormat().str().c_str() 
                              , format.str().c_str()
                              );
                    
                    LM_W(( message.c_str() ));
                    
                    // Send a trace to all delilahs
                    engine::Notification* notification = new engine::Notification(notification_samson_worker_send_trace);
                    notification->environment.set("message",message);
                    notification->environment.set("context","queues");
                    engine::Engine::shared()->notify( notification );
                    
                    return;
                }
            }
            
            // Update statistics
            KVHeader header = block->getHeader();
            rate.push( header.info.kvs   , header.info.size );

            
            // Push data to stream out connections 
            // Note that once inside queue, we do not know what to sent, so now it is the time
            au::map< size_t , StreamOutConnection >::iterator it_connections;
            for( it_connections = streamManager->stream_out_connections.begin() 
                ; it_connections != streamManager->stream_out_connections.end() 
                ; it_connections++ )
                it_connections->second->push( name , &block_list );
            
            
            
            // Add to the list for this queue
            list->add( block );
            
        }   
        
        void Queue::push( BlockList *list )
        {
            au::list< Block >::iterator b;
            for (b = list->blocks.begin() ; b != list->blocks.end() ; b++ )
                push( *b );
        }
        
        void Queue::check_format()
        {
            
            if ( list->isEmpty() )
            {
                format = KVFormat("*","*");
                
            }
            else if( format == KVFormat("*","*") )
            {
                Block* block = *list->blocks.begin();
                format = block->header->getKVFormat();  // Get the format of the first one...
            }
        }
        
        
     
        
        void Queue::getInfo( std::ostringstream& output)
        {
            au::xml_open(output, "queue");

            au::xml_simple(output, "name", name );

            // Information about the format
            format.getInfo(output);

            // Information about content
            BlockInfo block_info;
            update( block_info );
            block_info.getInfo(output);
            
            au::xml_simple( output , "environment" , environment.getEnvironmentDescription() );
                        
            au::xml_close(output, "queue");
        }
        
        void Queue::update( BlockInfo& block_info )
        {
            list->update( block_info );
            
        }
        
        void Queue::setProperty( std::string property , std::string value )
        {
            environment.set( property , value );
            
            if( property == "priority" )
            {
                int p = atoi( value.c_str() );
                LM_W(("Setting priority to queue %d" , p));
                list->setPriority( p );
            }
            
        }

        void Queue::unsetProperty( std::string property )
        {
            environment.unset( property );
        }
        
        void Queue::replaceAndUnlock( BlockList *from , BlockList *to )
        {
            replace( from , to );
            unlock(from);
            
            check_format();
        }
        
        void Queue::replace( BlockList *from , BlockList *to )
        {
            // Replace a list of blocks by another list
            list->replace( from , to );
            
            check_format();

        }

        void Queue::removeAndUnlock( BlockList *_list )
        {
            list->remove( _list );
            unlock( _list );
            
            check_format();
        }
        
        void Queue::remove ( BlockList *_list )
        {
            list->remove( _list );
            check_format();
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
        
        void Queue::getBlocksForKVRange( KVRange range , BlockList *outputBlockList )
        {
            au::list< Block >::iterator block_it;
            for ( block_it = list->blocks.begin() ; block_it != list->blocks.end() ; block_it++ )
            {
                Block *block = *block_it;
                KVRange block_range = block->getKVRange();
                
                if( block_range.overlap( range ) )
                    outputBlockList->add( block );
            }
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
            au::ExecesiveTimeAlarm alarm("Queue::review");
            
            // Review the size contained in this queue if there is a limit defined....
            size_t max_size = environment.getSizeT( "max_size" , 0 );
            if( max_size > 0 )
            {
                BlockList tmp_block_list;   // Temporal list to put extracted blocks...
                while( list->getBlockInfo().size > max_size )
                    tmp_block_list.extractBlockFrom( list );
            }
            
            // Review if new blocks shold be created
            buffer_accumulator.review();
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
                        num_blocks++;
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
        
        size_t Queue::getAccumulatedTotalSize()
        {
            return buffer_accumulator.getTotalSize();
        }
        
        void Queue::fill( samson::network::CollectionRecord* record , Visualization* visualization )
        {
            
            // Get the options
            VisualitzationOptions options = visualization->options;
              
            // Get block information for this queue
            BlockInfo blockInfo;
            update( blockInfo );

            
            // Last component of name
            /*
            std::string last_component_name = name;
            size_t pos = name.find_last_of("/");
            if( pos != std::string::npos )
                last_component_name = name.substr( pos+1 );
             */

            
            if( options == stream_block )
            {
                std::string type = visualization->environment.get("type" , "unknown" );
                
                if( type == "input" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs" , blockInfo.info.kvs , "f=uint64,sum" );
                    add( record , "In: size" , blockInfo.info.size , "f=uint64,sum" );
                    add( record , "State: #kvs" , "" , "f=uint64,sum" );
                    add( record , "State: size" , "" , "f=uint64,sum" );
                    add( record , "Out: #kvs" , "" , "f=uint64,sum" );
                    add( record , "Out: size" , "" , "f=uint64,sum" );
                }

                if( type == "state" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs" , "" , "f=uint64,sum" );
                    add( record , "In: size" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs" , blockInfo.info.kvs , "f=uint64,sum" );
                    add( record , "State: size" , blockInfo.info.size , "f=uint64,sum" );
                    add( record , "Out: #kvs" , "" , "f=uint64,sum" );
                    add( record , "Out: size" , "" , "f=uint64,sum" );
                }
                
                if( type == "output" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs" , "" , "f=uint64,sum" );
                    add( record , "In: size" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs" , "" , "f=uint64,sum" );
                    add( record , "State: size" , "" , "f=uint64,sum" );
                    add( record , "Out: #kvs" , blockInfo.info.kvs , "f=uint64,sum" );
                    add( record , "Out: size" , blockInfo.info.size , "f=uint64,sum" );
                }
                
                if( type == "internal_state" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs" , "" , "f=uint64,sum" );
                    add( record , "In: size" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs" , blockInfo.info.kvs , "f=uint64,sum" );
                    add( record , "State: size" , blockInfo.info.size , "f=uint64,sum" );
                    add( record , "Out: #kvs" , "" , "f=uint64,sum" );
                    add( record , "Out: size" , "" , "f=uint64,sum" );
                }
                
                return;
            }
            
            
            if( options == stream_block_rates )
            {
                std::string type = visualization->environment.get("type" , "unknown" );
                
                if( type == "input" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs/s" , rate.get_rate_kvs() , "f=uint64,sum" );
                    add( record , "In: size/s" , rate.get_rate_size() , "f=uint64,sum" );
                    add( record , "State: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "State: size/s" , "" , "f=uint64,sum" );
                    add( record , "Out: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "Out: size/s" , "" , "f=uint64,sum" );
                }
                
                if( type == "state" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "In: size/s" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs/s" , rate.get_rate_kvs() , "f=uint64,sum" );
                    add( record , "State: size/s" , rate.get_rate_size() , "f=uint64,sum" );
                    add( record , "Out: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "Out: size/s" , "" , "f=uint64,sum" );
                }
                
                if( type == "output" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "In: size/s" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "State: size/s" , "" , "f=uint64,sum" );
                    add( record , "Out: #kvs/s" , rate.get_rate_kvs() , "f=uint64,sum" );
                    add( record , "Out: size/s" , rate.get_rate_size() , "f=uint64,sum" );
                }
                
                if( type == "internal_state" )
                {
                    add( record , "type" , type , "left,different" );
                    add( record , "name" , name , "left,different" );
                    add( record , "In: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "In: size/s" , "" , "f=uint64,sum" );
                    add( record , "State: #kvs/s" ,rate.get_rate_kvs() , "f=uint64,sum" );
                    add( record , "State: size/s" , rate.get_rate_size() , "f=uint64,sum" );
                    add( record , "Out: #kvs/s" , "" , "f=uint64,sum" );
                    add( record , "Out: size/s" , "" , "f=uint64,sum" );
                }
                
                return;
            }
            
            
            // Spetial case for properties...
            if( options == properties )
            {
                add( record , "name" , name , "left,different" );
                add( record , "properties" , environment.getEnvironmentDescription() , "left,different" );
                return;
            }
            
            
            // Normal lists
            
            add( record , "name" , name , "left,different" );
            add( record , "#kvs" , blockInfo.info.kvs , "f=uint64,sum" );
            add( record , "size" , blockInfo.info.size , "f=uint64,sum" );
            
            if( ( options == normal ) || (options == all ) )
            {
                add( record , "key" , format.keyFormat  , "different");
                add( record , "value" , format.valueFormat , "different" );
            }
            
            if( ( options == rates ) || (options == all ) )
            {

                add( record , "Total #kvs"  , (size_t)rate.get_total_kvs() , "f=uint64,sum" );
                add( record , "Total size"  , (size_t)rate.get_total_size() , "f=uint64,sum" );
                
                add( record , "#kvs/s"  , (size_t) rate.get_rate_kvs() , "f=uint64,sum" );
                add( record , "Bytes/s" , (size_t) rate.get_rate_size() , "f=uint64,sum" );
            }
            
            if( options == blocks )
            {
                add( record , "#Blocs"     , blockInfo.num_blocks , "f=uint64,sum" );
                
                if( format.isTxt() )
                    add( record , "Fragmentation" , "0" , "f=per,different" );
                else
                    add( record, "Fragmentation", list->getFragmentationFactor() , "f=per,different");
                
                add( record , "Size"       , blockInfo.size , "f=uint64,sum" );
                add( record , "on Memory"  , blockInfo.size_on_memory , "f=uint64,sum" );
                add( record , "on Disk"    , blockInfo.size_on_disk , "f=uint64,sum" );
                add( record , "Locked"     , blockInfo.size_locked , "f=uint64,sum" );
                
                if( blockInfo.num_blocks == 0 )
                {
                    add( record , "Time from"  , "-" , "f=timestamp,different" );
                    add( record , "Time to"    , "-" , "f=timestamp,different" );
                }
                else
                {
                    add( record , "Time from"  , au::str_timestamp(blockInfo.min_time) , "different" );
                    add( record , "Time to"    , au::str_timestamp(blockInfo.max_time) , "different" );
                }
            }
            
        }
        
    }
}

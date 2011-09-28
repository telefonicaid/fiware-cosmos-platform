
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "BlockBreak.h"                             // samson::stream::BlockBreak
#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList
#include "Queue.h"                                  // samson::stream::Queue
#include "StreamManager.h"                          // samson::stream::StreamManager

#include "BlockBreakQueueTask.h"                    // Own interface

namespace samson {
    namespace stream {
        
        
#pragma mark BlockBreakQueueTask        

        
        BlockBreakQueueTask::BlockBreakQueueTask( size_t _id , std::string _queue_name , int _num_divisions  )
        : SystemQueueTask( _id , au::str("BlockBreak ( %lu )" , _id ) )
        {
            queue_name = _queue_name;
            num_divisions = _num_divisions;
            
            setProcessItemOperationName( "system.BlockBreak" );
        }
        
        
        void BlockBreakQueueTask::run()
        {
            BlockList *list = getBlockList("input_0");
            
            
            // structure used to point to all the blocks used in this operation
            num_blocks  = list->blocks.size();
            files       = (KVFile*) malloc( sizeof( KVFile ) * num_blocks );
            
            // Get all pointers to correct datas
            au::list<Block>::iterator b;
            int pos = 0;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                files[pos++].init( (*b)->buffer->getData() );
            
            
            for (int r = 0 ; r < num_divisions ; r++ )
            {
                // Range covered in this iteration
                KVRange range = rangeForDivision( r , num_divisions );
                
                KVInfo total_info;
                total_info.clear();
                
                int file_begin = 0;
                for (int f = 0 ; f < num_blocks ; f++)
                {
                    KVInfo info_for_range = selectRange( files[f].info , range );
                    
                    if( total_info.canAppend( info_for_range ) )
                    {
                        total_info.append( info_for_range );
                    }
                    else
                    {
                        // Create a block with this elements
                        createBlock( range , file_begin , f );
                        
                        // Prepare for the next one
                        file_begin = f;
                        total_info.clear();
                        total_info.append( info_for_range );
                        
                    }
                }
                
                // Create the last block
                createBlock( range, file_begin , num_blocks );
                
            }
            
            free( files);
        }
        
        void BlockBreakQueueTask::createBlock( KVRange range , int file_begin , int file_end )
        {
            // Compute size of the generated buffer
            KVInfo info;
            for (int f = file_begin ; f < file_end ; f++ )
                info.append( selectRange( files[f].info , range ) );
            
            if ( info.size == 0 )
                return;
            
            // total size of the new block
            size_t size = sizeof( KVHeader ) + sizeof(KVInfo)*KVFILE_NUM_HASHGROUPS + info.size;
            
            // Alloc buffer
            engine::Buffer *buffer = engine::MemoryManager::shared()->newBuffer( "block_break", size , MemoryBlocks );
            buffer->setSize( size );
            
            KVFile file( buffer->getData() );
            
            // Header
            memcpy( file.header , files[0].header ,  sizeof( KVHeader ) );
            file.header->info = info;
            
            //KVInfo
            clear( file.info );
            
            size_t offset = 0;
            for (int hg = range.hg_begin ; hg < range.hg_end ; hg++)
            {
                for (int f = file_begin ; f < file_end ; f++ )
                {
                    memcpy(file.data + offset, files[f].data + files[f].offset(hg) , files[f].info[hg].size );
                    offset += files[f].info[hg].size;
                    
                    file.info[hg].append( files[f].info[hg] );
                }
            }

            // Recompute range...
            //file.header->range.setFrom( file.info );
            file.header->range = range;
            
            
            // Collect the output buffer
            outputBuffers.push_back(buffer);
            
        }
        
        void BlockBreakQueueTask::finalize( StreamManager* streamManager )
        {
            
            // Create the list with the outputs
            BlockList *tmp = new BlockList("tmp_block_break_outputs");
            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                tmp->createBlock( outputBuffers[i] );
            
            BlockList *originalBlockList = getBlockList("input_0");
            
            // Get the queue we are working on
            Queue*queue = streamManager->getQueue( queue_name );
            
            if ( queue )
                queue->replaceAndUnlock( originalBlockList , tmp );
            
            // Detele the temporal list used here
            delete tmp;
        }
        
        
    }
    
}
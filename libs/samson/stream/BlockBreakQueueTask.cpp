
#include "engine/Engine.h"
#include "engine/Notification.h"

#include "samson/common/NotificationMessages.h"     // notification_samson_worker_send_packet
#include "samson/common/MemoryTags.h"               // MemoryBlocks

#include "samson/network/Packet.h"                  // network::Packet

#include "samson/module/ModulesManager.h"           // samson::ModulesManager


#include "Block.h"                                  // samson::stream::Block
#include "BlockList.h"                              // samson::stream::BlockList

#include "Queue.h"                                  // samson::stream::Queue

#include "StreamManager.h"                          // samson::stream::StreamManager

#include "BlockBreakQueueTask.h"                    // Own interface

namespace samson {
    namespace stream {
        
        
#pragma mark BlockBreakQueueTask        
        
        BlockBreakQueueTask::BlockBreakQueueTask( size_t _id , std::string _queue ) : SystemQueueTask( _id )
        {
            queue = _queue;
        }
        
        void BlockBreakQueueTask::addKVRange( KVRange range )
        {
            ranges.push_back( range );
        }
        
        void BlockBreakQueueTask::run()
        {
            BlockList *list = getBlockList("input_0");
            
            LM_M(("Running block break with %lu blocks and %lu ranges" , list->getNumBlocks() , ranges.size()));
            
            // structure used to point to all the blocks used in this operation
            num_blocks  = list->blocks.size();
            files       = (KVFile*) malloc( sizeof( KVFile ) * num_blocks );
            
            // Get all pointers to correct datas
            au::list<Block>::iterator b;
            int pos = 0;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
                files[pos++].init( (*b)->buffer->getData() );
            
            
            for (int r = 0 ; r < (int) ranges.size() ; r++ )
            {
                // Range covered in this iteration
                KVRange range = ranges[r];
                
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
            file.header->range.setFrom( file.info );
            
            
            // Collect the output buffer
            outputBuffers.push_back(buffer);
            
        }
        
        void BlockBreakQueueTask::finalize( StreamManager* streamManager )
        {
            BlockList *tmp = new BlockList("tmp_block_break_outputs");
            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                tmp->createBlock( outputBuffers[i] );
            
            Queue* _queue = streamManager->getQueue( queue );

            // Remove the blocks used as inputs for this operation
            _queue->pending->remove( getBlockList("input_0") );
                                    
            // Puhs new blocks obtained in this operation
            _queue->push( tmp );

            // Detele the temporal list used here
            delete tmp;
            
            LM_M(("Finalize method of the block-break operation %lu output buffers" , outputBuffers.size() ));
        }
        
        
    }
    
}
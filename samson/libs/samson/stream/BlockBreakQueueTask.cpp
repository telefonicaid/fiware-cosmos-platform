/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

#include "logMsg/logMsg.h"                          // LM_M , LM_X

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

        
        BlockBreakQueueTask::BlockBreakQueueTask( size_t _id , std::string _queue_name , size_t _output_operation_size )
        : SystemQueueTask( _id , au::str("BlockBreak ( %lu )" , _id ) )
        {

            queue_name = _queue_name;
            setProcessItemOperationName( "system.BlockBreak" );
            output_operation_size = _output_operation_size;
        }

        BlockBreakQueueTask::~BlockBreakQueueTask()
        {
            files.clearVector();
        }
        
        void BlockBreakQueueTask::run()
        {
            // Get the input list of blocks
            BlockList *list = getBlockList("input");
            
            // Get all pointers to correct datas ( using KVFile structure )
            au::list<Block>::iterator b;
            for ( b = list->blocks.begin() ; b != list->blocks.end() ; b++)
            {
                engine::Buffer * buffer = (*b)->buffer_container.getBuffer();
                files.push_back( new KVFile( buffer->getData() ) );
            }
            
            // Compute the size for each hg
            FullKVInfo *info = (FullKVInfo*) malloc( KVFILE_NUM_HASHGROUPS * sizeof(FullKVInfo) );
            
            for( int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++ )
            {
                info[hg].clear();
                for( size_t f = 0 ; f < files.size() ; f++ )
                    info[hg].append( files[f]->getKVInfoForHashGroup(hg) );
            }
            
            // Create (aprox) 64Mb ouptut blocks....
            size_t accumulated_size = 0;
            int from_hg = 0;  
            int to_hg = 0;
            
            while( to_hg < KVFILE_NUM_HASHGROUPS )
            {
                if( (to_hg > from_hg) && (( accumulated_size + info[to_hg].size ) > output_operation_size ))
                {
                    createBlock( from_hg, to_hg+1 );
                    from_hg = to_hg + 1;
                    to_hg = from_hg;
                    accumulated_size = 0;
                }
                else
                {
                    accumulated_size += info[to_hg].size;
                    to_hg++;
                }
            }

            // Create the last block
            if( to_hg > from_hg )
                createBlock( from_hg, to_hg );
            
            free(info);
            
        }
        
        void BlockBreakQueueTask::createBlock( int hg_begin , int hg_end )
        {
            

            // Compute size of the generated buffer
            KVInfo info;
            for( int hg = hg_begin ; hg < hg_end ; hg++ )
                for (size_t f = 0 ; f < files.size() ; f++ )
                    info.append( files[f]->getKVInfoForHashGroup(hg) );
            
            if ( info.size == 0 )
                return; // No block is generated
            
            // total size of the new block
            size_t size = sizeof( KVHeader ) + info.size;
            
            // Get the buffer from the memory manager & set the correct size
            engine::Buffer *buffer = engine::MemoryManager::shared()->createBuffer( "block_break", "break", size );
            buffer->setSize( size );

            // Pointer to the header
            KVHeader * header = (KVHeader*) buffer->getData();
            char* data = buffer->getData() + sizeof(KVHeader);
            
            // Copy header from the first block
            memcpy( header , files[0]->getKVHeader() ,  sizeof( KVHeader ) );
            
            // Change the size to the real one
            header->setInfo(info);
            
            // Temporal vector of KVInfo's
            KVInfo* info_vector = new KVInfo[ KVFILE_NUM_HASHGROUPS ];
            
            // Copy content            
            size_t offset = 0;
            for (int hg = hg_begin ; hg < hg_end ; hg++)
            {
                for (size_t f = 0 ; f < files.size() ; f++ )
                {
                    KVInfo _info = files[f]->getKVInfoForHashGroup(hg);
                    
                    memcpy(data + offset, files[f]->dataForHashGroup(hg) , _info.size );
                    offset += _info.size;
                    
                    info_vector[hg].append( _info );
                }
            }
            
            // Adjust the range
            //file.header->range = KVRange(hg_begin , hg_end );
            header->range.setFrom( info_vector ); // Adjust the range of generated bocks
            
            // Collect the output buffer ( all of the are retained )
            outputBuffers.push_back(buffer);
            
        }
        
        void BlockBreakQueueTask::finalize( StreamManager* streamManager )
        {
        	LM_T(LmtBlockManager,("BlockBreakQueueTask::starts finalize for (%lu) (%s) outputBuffers.size():%d, creating blocks", id, concept.c_str(), outputBuffers.size()));

            // Create the list with the outputs
            BlockList tmp("tmp_block_break_outputs");

            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                tmp.createBlock( outputBuffers[i] );

            // Get the queue and push generated blocks
            Queue*queue = streamManager->getQueue( queue_name );
            if ( queue )
                queue->push(&tmp);
            
            LM_T(LmtBlockManager,("BlockBreakQueueTask::ends finalize for (%lu) (%s) outputBuffers.size():%d, creating blocks", id, concept.c_str(), outputBuffers.size()));
            
            
            // Clear and release buffers in outputBuffers
            for (size_t i = 0 ; i < outputBuffers.size() ; i++ )
                outputBuffers[i]->release();
            outputBuffers.clear();
            
            
        }
        
        
    }
    
}

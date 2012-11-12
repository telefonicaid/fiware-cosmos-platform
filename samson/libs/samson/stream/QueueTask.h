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
#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>              // std::set
#include <set>
#include <sstream>

#include "au/string.h"                              // au::Format

#include "engine/ProcessItem.h"                     // engine::ProcessItem

#include "samson/common/MessagesOperations.h"

#include "samson/stream/QueueTaskBase.h"            // parent class samson::stream::QueueTaskBase
#include "samson/stream/BlockList.h"                // stream::BlockList
#include "samson/stream/StreamProcessBase.h"        // parent class public StreamProcessBase
#include "samson/stream/Block.h"                    // samson::Stream::Block
#include "samson/stream/BlockInfo.h"                // struct BlockInfo

namespace samson {
    namespace stream {

        class Block;

        
        // Base class for all the stream tasks ( parser , map , reduce , parseOut )
        class QueueTask : public StreamProcessBase , public QueueTaskBase
        {
            
        public:

            // Constructor
            QueueTask( size_t _id , StreamOperationBase* streamOperation  );
            
            // Function executed just before task is deleted in the main thread
            virtual void finalize(StreamManager* streamManager) { if (streamManager == NULL) return; };
            
            // Get information for monitoring
            void getInfo( std::ostringstream& output);

            //  Set the input size to make sure ProcessItem can monitor performance of this operation
            void setWorkingSize()
            {
                // Note: We are consudering input form "input_0" since in update-state operations
                // we are only interested in input data ( nor state data )
                
                BlockInfo block_info;
                getBlockList("input_0")->update( block_info );
                setProcessItemWorkingSize( block_info.size );
            }

            
            void fill( samson::network::CollectionRecord* record , VisualitzationOptions options );
            
        };
        
    }
 
}

#endif

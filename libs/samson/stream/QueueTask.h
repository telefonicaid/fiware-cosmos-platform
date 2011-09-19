#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>              // std::set
#include <set>
#include <sstream>

#include "au/string.h"                              // au::Format

#include "engine/ProcessItem.h"                     // engine::ProcessItem

#include "samson/stream/QueueTaskBase.h"            // parent class samson::stream::QueueTaskBase
#include "samson/stream/BlockList.h"                // stream::BlockList
#include "samson/stream/StreamProcessBase.h"        // parent class public StreamProcessBase
#include "samson/stream/Block.h"                    // samson::Stream::Block

namespace samson {
    namespace stream {

        class Block;

        
        // Base class for all the stream tasks ( parser , map , reduce , parseOut )
        class QueueTask : public StreamProcessBase , public QueueTaskBase
        {
            
            FullKVInfo queue_task_info;         // total information covered by this task
            
        public:

            // Constructor
            QueueTask( size_t _id , const network::StreamOperation& streamOperation  );
            
            // Overloaded method for ProcessItem
            virtual std::string getStatus()
            {
                return au::str("No task description for task %lu", id );
            }
            
            // Function executed just before task is deleted in the main thread
            virtual void finalize(){};
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);

            //  Set the input size to make sure ProcessItem can monitorize performance of this operation
            void setWorkingSize()
            {
                BlockInfo block_info;
                update( block_info );
                setProcessItemWorkingSize( block_info.size );
            }
            
        };
        
    }
 
}

#endif

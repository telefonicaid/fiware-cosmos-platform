#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>              // std::set
#include <set>
#include <sstream>

#include "au/Format.h"                              // au::Format

#include "engine/ProcessItem.h"                     // engine::ProcessItem

#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/Block.h"                    // samson::Stream::Block
#include "samson/stream/BlockMatrix.h"              // samson::stream::BlockMatrix

namespace samson {
    namespace stream {

        class Block;
        
        // Base class for all the stream tasks ( parser , map , reduce , parseOut )
        
        class QueueTask : public StreamProcessBase
        {
            
            FullKVInfo queue_task_info;         // total information covered by this task
            
        public:
            
            size_t id;                          // Id of the operation
            
            BlockMatrix matrix;                 // Matrix of blocks involved in this operation
            
            QueueTask( size_t _id , network::StreamQueue * streamQueue  ) : StreamProcessBase( _id , streamQueue )
            {
                // Set the id of this task
                id = _id;
                
                // Set in the environemtn variables
                environment.setSizeT("id",id);
            }

            QueueTask( size_t _id , PopQueue *_pq ) : StreamProcessBase( _id , _pq )
            {
                // Set the id of this task
                id = _id;

                // Set in the environemtn variables
                environment.setSizeT("id",id);
            }
            
            
            virtual ~QueueTask()
            {
            }
            
            
            // Funciton to check if all the blocks are in memory
            bool ready();
            
            // retain and release all the blocks considered in this task
            void retain();
            void release();
            
            // Lock and unlock all the blocks used in this operations
            bool lock();
            void unlock();
            
            virtual std::string getStatus()
            {
                return au::Format::string("No task description for task %lu", id );
            }
            
            // Function to return the process to be executed
            virtual StreamProcessBase* getStreamProcess()=0;

            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
        };
 
        
        
    }
 
}

#endif
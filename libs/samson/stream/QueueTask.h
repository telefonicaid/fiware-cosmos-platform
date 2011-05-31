#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>              // std::set
#include "au/Format.h"      // au::Format
#include <set>
#include <sstream>
#include "Block.h"  
#include "engine/ProcessItem.h"
#include "samson/stream/StreamProcessBase.h"        // parent class 

namespace samson {
    namespace stream {

        class Block;
        
        // Base class for all the stream tasks ( parser , map , reduce , parseOut )
        
        class QueueTask
        {

        protected:
            
            friend class QueueTaskManager;      // Manager 
            std::set< Block* > blocks;          // List of blocks involved in the operation ( retained by this operation )
            size_t id;                          // Id of the operation

            
        public:
            
            QueueTask(  )
            {
            }
            
            virtual ~QueueTask()
            {
            }
            
            void add( Block * b );
            
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

        };
 
        
        
    }
 
}

#endif
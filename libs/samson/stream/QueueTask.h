#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

#include <set>              // std::set
#include "au/Format.h"      // au::Format
#include <set>
#include <sstream>
#include "Block.h"  
#include "engine/ProcessItem.h"
#include "samson/worker/StreamProcessBase.h"

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
            
            ~QueueTask()
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
            
        };
    
        
        // Parser QueueTask
        
        class ParserQueueTask : public QueueTask , public StreamProcessBase
        {
            
        public:
            
            ParserQueueTask( network::StreamQueue * streamQueue  ) : StreamProcessBase( key_value , streamQueue )
            {
            }
            
            virtual std::string getStatus()
            {
                std::ostringstream output;
                output << "[" << id << "] ";
                output << "Parser " << streamQueue->operation() << " processing " << blocks.size() << " blocks with " << au::Format::string( getSize( blocks ) );
                return output.str();
            }
            
            void generateKeyValues( KVWriter *writer )
            {
                LM_M(("Running parser over %d blocks" , (int)blocks.size() ));
            }
            
        };
        
        
    }
    
}

#endif
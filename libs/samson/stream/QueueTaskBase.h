#ifndef _H_SAMSON_QUEUE_TASK_BASE
#define _H_SAMSON_QUEUE_TASK_BASE

#include <set>              // std::set
#include <set>
#include <sstream>

#include "au/string.h"                              // au::Format

#include "engine/ProcessItem.h"                     // engine::ProcessItem

#include "samson/stream/BlockList.h"                // stream::BlockList
#include "samson/stream/StreamProcessBase.h"        // parent class 
#include "samson/stream/Block.h"                    // samson::Stream::Block

namespace samson {
    namespace stream {
        
        class Block;
        
        
        // Class manager to deal with lists of blocks that shoudl be retained when ready
        
        class QueueTaskBase
        {
            // Flag activated when ready has return true
            bool ready_flag;
            
            // All kind of inputs ( mapped by name: input_1, input_2 , .... )
            au::map<std::string, BlockList > blockLists;
            
            // List to block all inputs for this task
            BlockList* lockBlockList;
            
        protected:
            
            size_t id;                          // Id of the operation
            
        public:
            
            au::Cronometer creation_cronometer;            // Creationg cronometer
            std::string queue_task_state;                  // A bit more information about state of this queue task
            void setQueueTaskState( std::string _queue_task_state )
            {
                queue_task_state = _queue_task_state;
            }

        public:
            
            // Constructor and destructor
            QueueTaskBase( size_t id );
            virtual ~QueueTaskBase();
            
            // Funciton to check if all the blocks are in memory 
            bool ready();
            
            // Get a particular input block-list
            BlockList* getBlockList( std::string name );
            
            // Get information function
            void getInfo( std::ostringstream& output);
            
            // update information contained in this operation
            void update( BlockInfo &block_info);
            
            // Get the id of this task
            size_t getId();
            
            void touch_input_blocks()
            {
                au::map<std::string, BlockList >::iterator it_blockLists;
                for( it_blockLists = blockLists.begin() ; it_blockLists != blockLists.end() ; it_blockLists++ )
                    it_blockLists->second->touch();
            }
            
        };
        
    }
}

#endif
            
        
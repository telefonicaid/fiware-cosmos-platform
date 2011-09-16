#ifndef _H_SAMSON_QUEUE_TASK
#define _H_SAMSON_QUEUE_TASK

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
        
        // Base class for all the stream tasks ( parser , map , reduce , parseOut )
        
        class QueueTask : public StreamProcessBase
        {
            
            FullKVInfo queue_task_info;         // total information covered by this task
            
        public:
            
            size_t id;                          // Id of the operation
            
            BlockList *list;                    // List of blocks to be processed
            BlockList *state;                   // blocks of the state ( in reduce-like operations )

            BlockList *list_lock;               // A list to block the input blocks
            BlockList *state_lock;              // A list to block the state
            
            bool ready_flag;
            
            QueueTask( size_t _id , const network::StreamOperation& streamOperation  ) : StreamProcessBase( _id , streamOperation )
            {
                // Set the id of this task
                id = _id;
                
                // Set in the environemtn variables
                environment.setSizeT("id",id);
                
                // Create the block list
                list  = new BlockList( au::str("Task %lu input" , id ) , id , false );
                state  = new BlockList( au::str("Task %lu state" , id ) , id , false );
                
                // Create the lock block list
                list_lock  = new BlockList( au::str("Task %lu input lock" , id ) , id , true );
                state_lock  = new BlockList( au::str("Task %lu state lock" , id ) , id , true );
                
                ready_flag = false;
                
            }
            
            
            virtual ~QueueTask()
            {
                delete list;
                delete state;
                
                delete list_lock;
                delete state_lock;
            }
            
            
            // Funciton to check if all the blocks are in memory 
            bool ready();
            
            virtual std::string getStatus()
            {
                return au::str("No task description for task %lu", id );
            }
            
            // Function executed just before task is deleted
            virtual void finalize(){};
            
            // Get information for monitorization
            void getInfo( std::ostringstream& output);
            
        };
 
        
        
    }
 
}

#endif


#ifndef _H_SAMSON_STREAM_QUEUE
#define _H_SAMSON_STREAM_QUEUE

#include <string>                       // std::string

#include "au/list.h"                    // au::list
#include "au/Environment.h"             // au::Environment

#include "samson/common/coding.h"       // KVFullInfo
#include "samson/common/samson.pb.h"    // samson::network::...

namespace samson {
    
    namespace stream
    {
        class QueueItem;
        class BlockList;
        class ReduceQueueTask;
        class StreamManager;
        class Block;
        
        class Queue
        {
            
            friend class QueueItem;
            friend class StreamManager;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class BlockList;
            
            // Pointer to StreamManager
            StreamManager* streamManager;
            
            // Name of this queue
            std::string name;

            // List of state items in this state
            au::list< QueueItem > items;
            
            // List of blokcs pending to be broken into mini-blocks for this queue
            BlockList *pending;
            
            // Environment properties of this queue
            au::Environment environment;
            
            // Flag to avoid creating loking for operations
            bool paused;

            // Format of this queue
            KVFormat format;
            
        public:
            
            // Constructor and destructor
            Queue( std::string _name , StreamManager* _streamManager ,  int num_items );
            ~Queue();

            // Push content form a block list ( do not remove original list )
            void push( BlockList *list );
            
            // Function to check if any of the items is working ( if so, it can not be removed , clered or whatever )
            bool isWorking();
            
            // Get information about the queue
            void update( BlockInfo& block_info );
                        
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
            // Copy content
            void copyFrom( Queue* _queue);
            
            // Clear and distribute in a number of divisions
            void clearAndDivide( int num_divisions );
            
            // Set a property
            void setProperty( std::string property , std::string value );
            
        private:
            
            void divide( QueueItem *item , QueueItem *item1 , QueueItem *item2 );

            void push( Block *block );
            
            
        };
        
        
    }
}

#endif
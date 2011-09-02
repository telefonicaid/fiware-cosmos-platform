
#ifndef _H_SAMSON_STREAM_QUEUE
#define _H_SAMSON_STREAM_QUEUE

#include <string>           // std::string

#include "au/list.h"        // au::list

#include "samson/common/coding.h"       // KVFullInfo
#include "samson/common/samson.pb.h"    // samson::network::...

namespace samson {
    
    namespace stream
    {

        class QueueItem;
        class BlockList;
        class ReduceQueueTask;
        class QueuesManager;
        
        class Queue
        {
            
            friend class QueueItem;
            friend class QueuesManager;
            
            // Pointer to QueuesManager
            QueuesManager* qm;
            
            // Name of this queue
            std::string name;

            // List of state items in this state
            au::list< QueueItem > items;
            
            // Flag to avoid creating loking for operations
            bool paused;
            
        public:
            
            Queue( std::string _name , QueuesManager* _qm ,  int num_items );
            ~Queue();

            // Push content form a block list ( do not remove original list )
            void push( BlockList *list );
            
            //Distribute items in the same way
            void distributeItemsAs( Queue* otherQueue );
            
            //Check if distribution is the same
            bool isDistributedAs( Queue* otherQueue );
            
            // Function to check if any of the items is working ( if so, it can not be removed , clered or whatever )
            bool isWorking();
            
            // Get information about the queue
            FullKVInfo getFullKVInfo();
            FullKVInfo getWorkingFullKVInfo();
                        
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
        private:
            
            void divide( QueueItem *item , QueueItem *item1 , QueueItem *item2 );

            
        };
        
        
    }
}

#endif
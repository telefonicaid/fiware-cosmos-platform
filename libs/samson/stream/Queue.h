
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

            // List of blokcs contained in this queue
            BlockList *list;
            
            // Environment properties of this queue
            au::Environment environment;
            
            // Flag to avoid creating loking for operations
            bool paused;

            // Format of this queue
            KVFormat format;
            
            // Number of divisions for this queue ( all blocks should satify this at long term using block-break operations )
            int num_divisions;
            
            // List of ids for the blocks currently involved in a break operation / replace-reduce operation
            std::set<size_t> block_ids_locked;

        public:
            
            // Constructor and destructor
            Queue( std::string _name , StreamManager* _streamManager );
            ~Queue();

            // Push content form a block list ( do not remove original list )
            void push( BlockList *list );
            
            // Function to check if any of the items is working ( if so, it can not be removed , clered or whatever )
            bool isWorking();
            
            // Get information about the queue
            void update( BlockInfo& block_info );
                        
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
            // Set a property
            void setProperty( std::string property , std::string value );
            
            // Review if it is necessary to break or joint some blocks
            void review();
            
            // Notify that a block-break operation has finished
            void replaceAndUnlock( BlockList *from , BlockList *to );

            // Get blocks contained in a particular range for state and input ( update status operation )
            BlockList *getStateBlockListForRange( KVRange range );
            BlockList *getInputBlockListForRange( KVRange range , size_t max_size );
            
            void unlockStateBlockList( BlockList *_list );
            
        private:
            
            void divide( QueueItem *item , QueueItem *item1 , QueueItem *item2 );

            void push( Block *block );
            
            
            bool isBlockIdLocked( size_t block_id )
            {
                return ( block_ids_locked.find( block_id) != block_ids_locked.end() );
            }
            
            void lockBlockId( size_t block_id )
            {
                if( isBlockIdLocked( block_id) )
                    LM_W(("Reblocked block_id"));
                
                block_ids_locked.insert( block_id ); 
            }
            void unLockBlockId( size_t block_id )
            {
                block_ids_locked.erase( block_id );
            }
            
            
        };
        
        
    }
}

#endif

#ifndef _H_SAMSON_STREAM_QUEUE
#define _H_SAMSON_STREAM_QUEUE

#include <string>                       // std::string

#include "au/list.h"                    // au::list
#include "au/Environment.h"             // au::Environment
#include "au/Rate.h"                    // au::SimpleRate

#include "samson/common/coding.h"       // KVFullInfo
#include "samson/common/samson.pb.h"    // samson::network::...

#include "BlockInfo.h"                              // struct BlockInfo

namespace samson {
    
    namespace stream
    {
        class QueueItem;
        class BlockList;
        class ReduceQueueTask;
        class StreamManager;
        class Block;

        
        class BlockIdList
        {
            std::set<size_t> block_ids;

        public:
            
            void addIds( BlockList *list );
            void removeIds( BlockList *list );
            
            void addId( size_t id );
            void removeId( size_t id );
            
            size_t num_ids();            
            bool containsBlockId( size_t id );            
        };
        
        class Queue
        {
            
            friend class StreamManager;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class BlockList;
            friend class StreamOperation;
            
            // Pointer to StreamManager
            StreamManager* streamManager;
            
            // Name of this queue
            std::string name;
            
            // Environment properties of this queue
            au::Environment environment;

            // Format of this queue ( to filter input blocks )
            KVFormat format;
                        
            // Collections of ids for different block concepts
            BlockIdList lock_block_ids;
            
            // Number of divisions to meet ( block - break operations are scheduled if necessary ) 
            int num_divisions;
            
            // Monitoring of the input rate
            au::rate::Rate rate_kvs;
            au::rate::Rate rate_size;
            
        public:
            
            // List of blocks contained in this queue
            BlockList *list;
            
            // Constructor and destructor
            Queue( std::string _name , StreamManager* _streamManager );
            ~Queue();

            // Push content form a block list ( do not remove original list )
            void push( BlockList *list ); 
                        
            // Get information about the queue
            void update( BlockInfo& block_info );
                        
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
            // Set a property
            void setProperty( std::string property , std::string value );
            
            // Operations with block list
            void replaceAndUnlock( BlockList *from , BlockList *to );
            void removeAndUnlock( BlockList *list );
            void remove ( BlockList *list );
            
            void replace( BlockList *from , BlockList *to );
            bool lock ( BlockList *list );
            bool canBelock ( BlockList *list );
            void unlock ( BlockList *list );

            
            // Spetial function to get all the blocks for a particular range loking them
            void getBlocksForKVRange( KVRange range , BlockList *outputBlockList ); 
            bool getAndLockBlocksForKVRange( KVRange range , BlockList *outputBlockList ); 
            bool canLockBlocksForKVRange( KVRange range );
            
            // Check if this queue is fully divided in such a way that all blocks are divided perfectly
            bool isReadyForDivisions( ); 

            // Review if if is necesary to run block-break operations for this queue
            void review();
            
            void setMinNumDivisions( int _num_divisions )
            {
                if( num_divisions < _num_divisions )
                    num_divisions = _num_divisions;
            }
            
            KVFormat getFormat()
            {
                return format;
            }

            
            // Query function
            void fill( samson::network::CollectionRecord* record , VisualitzationOptions options );
            
        private:

            // Internal function to add a block to this queue
            void push( Block *block );
            
            // Get necesssary blocks to break
            void getBlocksToBreak( BlockList *outputBlockList  , size_t max_size );

            
        };
        
        
    }
}

#endif

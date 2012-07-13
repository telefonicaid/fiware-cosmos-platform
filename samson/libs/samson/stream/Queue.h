
#ifndef _H_SAMSON_STREAM_QUEUE
#define _H_SAMSON_STREAM_QUEUE

#include <string>                       // std::string

#include "au/containers/list.h"                    // au::list
#include "au/Environment.h"             // au::Environment
#include "au/Rate.h"                    // au::Rate

#include "samson/common/Rate.h"         // samson::Rate
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
        
        
#define MAX_TIME_TO_FLUSH_BUFFERS 0.5        // In time
#define MAX_SIZE_TO_FLUSH_BUFFERS 10000000   // In bytes
        

        class Queue;
        
        // Class to accumulate bufers before creating blocks
        class BufferAccumulator
        {
            // Queue to push data when finish
            Queue* queue;
            
            // Time since the last ( input buffers )
            au::Cronometer cronometer_review; 
                                              
            // List of buffers at the input of this queue ( waiting to be converted into a block )
            engine::BufferListContainer buffer_list_container;

        public:
            
            BufferAccumulator( Queue * _queue )
            {
                // Keep a pointer to the queue to push new blocks when created
                queue = _queue;
            }
            
            void push( engine::Buffer* buffer )
            {
                /*
                // If the incomming block is large enougth, we will create a block with it ( flushing previous buffers, if any )
                if( buffer->getSize() > MAX_SIZE_TO_FLUSH_BUFFERS )
                {
                    // Flush previous buffers if any
                    flush(); 

                    // Push this block into the local buffer
                    buffer_list_container.push_back(buffer);
                    
                    // Flush a new buffer with the new block
                    flush();
                    return;
                }
                 */
                
                // Init the clock if we are the first one...
                if( buffer_list_container.getNumBuffers() == 0 )
                    cronometer_review.reset();
                
                // Push the new buffer to the list
                buffer_list_container.push_back(buffer);
                
                // Review if the block has to be emited
                review();
            }
            
            void review()
            {
                // Review is called when engine has nothing to do... just flushing
                // Review if new blocks should be created
                size_t accumulated_size = buffer_list_container.getTotalSize();
                double time = cronometer_review.diffTime();
                
                if( accumulated_size > 0 )
                    if( ( accumulated_size > MAX_SIZE_TO_FLUSH_BUFFERS ) || ( time > MAX_TIME_TO_FLUSH_BUFFERS ) )
                        flush();
            }
            
            void flush();

            // Get total size accumulated here
            size_t getTotalSize();
            
        };
        
        
        class Queue
        {
            
            friend class StreamManager;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class BlockList;
            friend class StreamOperation;
            friend class BufferAccumulator;
            
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
            ::samson::Rate rate; // samson::Rate for kvs and size monitoring

            // Accumulator of buffers to create blcoks
            BufferAccumulator buffer_accumulator;
            
        public:
            
            // List of blocks contained in this queue
            BlockList *list;
            
            // Constructor and destructor
            Queue( std::string _name , StreamManager* _streamManager );
            ~Queue();

            // Push a new buffer
            void push( engine::Buffer * buffer );
                        
            // Flush accumulated buffers
            void flushBuffers();
            
            // Push blocks to this queue
            void push( BlockList *list );
            void push( Block *block );
            
            // Get information about the queue
            void update( BlockInfo& block_info );
                        
            // Getting XML information
            void getInfo( std::ostringstream& output);
            
            // Set a property
            void setProperty( std::string property , std::string value );
            void unsetProperty( std::string property );
            
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

            // Review stuff necessary for this queue
            void review();
            
            // Get total accumulated size
            size_t getAccumulatedTotalSize();
            
            void setMinNumDivisions( int _num_divisions )
            {
                if( num_divisions < _num_divisions )
                    num_divisions = _num_divisions;
            }
            
            KVFormat getFormat()
            {
                return format;
            }

            void check_format();
            
            // Query function
            void fill( samson::network::CollectionRecord* record , Visualization* visualization );
            
        private:

            
            // Get necesssary blocks to break
            void getBlocksToBreak( BlockList *outputBlockList  , size_t max_size );

            
        };
        
        
    }
}

#endif

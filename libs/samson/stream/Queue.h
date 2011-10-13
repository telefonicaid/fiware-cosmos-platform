
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
            
            friend class QueueItem;
            friend class StreamManager;
            friend class WorkerCommand;
            friend class BlockBreakQueueTask;
            friend class BlockList;
            friend class StreamOperation;
            
            // Pointer to StreamManager
            StreamManager* streamManager;
            
            // Name of this queue
            std::string name;

            // List of blokcs contained in this queue
            BlockList *list;
            
            // Environment properties of this queue
            au::Environment environment;
            
            // Number of updates when this is a state queue
            int updates;
            
            // Flag to avoid creating update_state operations
            bool paused;

            // Format of this queue
            KVFormat format;
            
            // Number of divisions for this queue ( all blocks should satify this at long term using block-break operations )
            int num_divisions;
            
            // Collections of ids for different block concepts
            BlockIdList breaking_block_ids;
            BlockIdList processing_block_ids;  // processing = parser / map / parseOut ( even reduce in non-update mode )

            // Divisions currently being updated ( only in reduce_state operations )
            // Note: it updating_divisions.size() > 0 --> Not possible to change num_divisions
            std::set<int> updating_divisions;
            
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
            void removeAndUnlock( BlockList *list );
            void replace( BlockList *from , BlockList *to );
            void unlock ( BlockList *list );
            void remove ( BlockList *list );

            // General function to know if a block is involved in any operation ( break or process )
            bool isBlockIdLocked( size_t id );

            // NORMAL PROCESSING
            // ------------------------------------------------------------------------------------
            BlockList* getInputBlockListForProcessing( size_t max_size );                             // Get input for an automatic processing
            BlockList* getInputBlockListForProcessing( size_t max_size , BlockIdList* used_blocks );  // Get input for an automatic processing
            
            // ------------------------------------------------------------------------------------
            
            // UPDATE STATE
            // ------------------------------------------------------------------------------------
            bool isQueueReadyForStateUpdate();                                              
            bool lockDivision( int division );                                              // Lock a particular division ( if possible )
            void unlockDivision( int division );                                            // Unlock a particular vision
            bool lockAllDivisions();
            void getStateBlocksForDivision( int division , BlockList *outputBlockList );    // Get the state for a state-update division
            BlockList *getInputBlockListForRange( KVRange range , size_t max_size );        // Get blocks for a range ( input of the reduce operation )
            // ------------------------------------------------------------------------------------
            
            int getNumUpdatingDivisions();
                        
            void setMinimumNumDivisions();
            
        private:

            // Internal function to add a block to this queue
            void push( Block *block );
            
            // Set a new number of visisions
            void setNumDivisions( int new_num_divisions );

            // Function to return the number of divisions based on the current size
            int getMinNumDivisions();
            
        };
        
        
    }
}

#endif
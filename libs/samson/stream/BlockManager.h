#ifndef _H_BLOCK_MANAGER
#define _H_BLOCK_MANAGER

#include "Block.h"                      // samson::stream::Block
#include "au/set.h"                     // au::set

#include "samson/common/Info.h"         // samson::Info

#include "engine/Object.h"  // engien::EngineListener
#include "engine/Object.h"              // engine::Object

#include <vector>

namespace samson {
    namespace stream {
        
        class BlockList;
        
        /**
         Manager of all the blocks running on the system
         */
        
        class BlockManager : public engine::Object 
        {
            
            std::list<Block*> blocks;       // List of blocks in the system ( ordered by priority )
            
            BlockManager();                 // Private constructor for singleton implementation
            ~BlockManager();
            
            size_t id;                      // Next id to give to a block

            int num_writing_operations;     // Number of writing operations ( low priority blocks )
            int num_reading_operations;     // Number of reading operations ( high priority blocks )
            
            size_t memory;                  // Total amount of memory used by all blocks
            size_t max_memory;              // Maximum amount of memory to always kept
            size_t max_memory_abs;          // Maximum amount of memory to be used
            
        public:

            // Singleton 
            static void init();
            static BlockManager* shared(); 
            static void destroy();
            
        public:

            // Auxiliar function to rise the next block identifier to not colide with previous blocks
            void setMinimumNextId( size_t min_id);
            
            // Function to get a new id for a block
            size_t getNextBlockId();
            
            // Add a block to the block manager
            // It is assumed block is NOT inside the list "blocks"
            
            void insert( Block* b );
            
            // Reconsider the position of this block in the global list 
            // it will affect who is loaded from disk / saved to disk or event flush out from memory
            // It is assumed block is inside the list "blocks"
            
            void check( Block* b );
            
        private:

            // Function to review pending read / free / write operations
            void _review();

            // Function used in the order of blocks
            std::list<Block*>::iterator _find_pos( Block *b );

            
        public:
            
            // Get a particular block ( only for debugging )
            Block* getBlock( size_t _id );

        public:
            
            virtual void notify( engine::Notification* notification );

        private:
            
            void _freeMemoryWithLowerPriorityBLocks( Block *b );

            
        public:
            
            void update( BlockInfo &block_info );
            void getInfo( std::ostringstream& output);


        public:
            void initOldFilesCheck();
            
        private:
            void reviewOldFiles();
            
            
        };
    }
}

#endif

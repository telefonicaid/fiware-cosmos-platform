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
            size_t max_memory;              // Maximum amount of memory to be used
            
        public:
                        
            static void init();

            static BlockManager* shared(); 
 
            static void destroy();
            
        public:

            size_t getNextBlockId()
            {
                return id++;
            }
            
            // Add a block to the block manager
            // It is assumed block is NOT inside the list "blocks"
            
            void insert( Block* b )
            {
                // Insert the new block in the rigth posistion
                blocks.insert( _find_pos(b),b );
                
                // Increase the amount of memory used by all blocks
                memory += b->size;
                
                // Review if new free,  write or reads are necessary
                _review();
            }
            
            // Reconsider the position of this block in the global list 
            // it will affect who is loaded from disk / saved to disk or event flush out from memory
            // It is assumed block is inside the list "blocks"
            
            void check( Block* b )
            {
                blocks.remove( b );
                
                // Check if the block should be removed, otherwise insert back in the list....
                
                // If it can be removed, just remove...
                if( b->canBeRemoved() )
                {
                    if( b->isContentOnMemory() )
                        memory -= b->size;
                    
                    if( b->isWriting() || b->isReading() )
                        LM_X(1,("Not allowed to remove an object that is reading or writting..."));
                    
                    delete b;
                    
                }
                else
                {
                    // Insert back in the global list of blocks
                    blocks.insert( _find_pos( b ) , b );
                }
                
                // Review if new free, write or reads are necessary
                _review();
                
            }
            
        private:

            // Function to review pending read / free / write operations
            void _review();

            // Function used in the order of blocks
            std::list<Block*>::iterator _find_pos( Block *b )
            {
                for (std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++)
                {
                    if( b->compare(*i) )
                        return i;
                }
                return blocks.end();
            }

            
        public:
            
            // Get a particular block ( only for debugging )
            Block* getBlock( size_t id )
            {
                for ( std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++ )
                    if( (*i)->id == id )
                        return *i;
                return NULL;
            }
            
            // debug message
            std::string str()
            {
                std::ostringstream output;
                output <<"BLockManager: <Reads: " << num_reading_operations << " // Writes: " << num_writing_operations << " > [ " << au::str( memory ) << " / " << au::str(max_memory) << " ] " ;
                for (std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; )
                {
                    output << (*i)->str();
                    
                    i++;
                    if( i!=blocks.end() )
                        output << ",";
                }
                return output.str();
                
            }

        public:
            
            virtual void notify( engine::Notification* notification );
            
            
        private:
            
            void _freeMemoryWithLowerPriorityBLocks( Block *b );

            
        public:
            
            void getInfo( std::ostringstream& output);
            
            
        };
    }
}

#endif

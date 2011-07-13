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
        
        /**
         Manager of all the blocks running on the system
         */
        
        class BlockManager : public engine::Object
        {
            
            std::list<Block*> blocks;       // List of blocks in the system ( ordered by priority )

            BlockManager();                 // Private constructor for singleton implementation
            
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

            // Add a block to the block manager
            void add( Block* b )
            {
                b->id = id++;       // Set the new id
                blocks.insert( _find_pos(b),b );
                
                // Increase the amount of memory used by all blocks
                memory += b->size;
                
                // Review if new free,  write or reads are necessary
                _review();
            }

            // --------------------------------------------------------
            // Retain - Release model
            // --------------------------------------------------------
            
            // Retain by a particular task ( it changes the preference in the list of blocks )
            void retain( Block* b , size_t task_id )
            {
                b->tasks.insert( task_id );

                // Reconsider the position of this block in the global list
                reorder( b );
                
            }

            // Release by a particular task ( it changes the preference in the list of blocks )
            void release( Block* b, size_t task_id )
            {
                b->tasks.erase( task_id );

                // Reconsider the position of this block in the global list
                reorder( b );

                // Review if the block should be removed
                reviewBlock( b );

            }
            
            void retain( Block* b )
            {
                b->retain_counter++;
            }
            
            void release( Block* b )
            {
                b->retain_counter--;

                // Review if the block should be removed
                reviewBlock( b );
                
                // Review pending tasks to be scheduled
                _review();
            }      
            
            void reviewBlock( Block *b )
            {
                // If no retained by anyone, remove...
                if( !b->isRetained() )
                {
                    if( b->isContentOnMemory() )
                        memory -= b->size;
                    
                    blocks.remove( b );
                    
                    if( b->isWriting() || b->isReading() )
                        LM_X(1,("Not allowed to remove an object that is reading..."));
                    
                    delete b;
                }
                
            }
            
            
            static bool lock( Block *block  )
            {
                if( ! block->isContentOnMemory() )
                    LM_X(1, ("Internal error loking blocks of memory"));
                block->lock_counter++;
                return true;   
            }
            
            static void unlock( Block *block   )
            {
                block->lock_counter--;
                if( block->lock_counter < 0 )
                    LM_X(1,("Internal error, Lock counter cannot be negative"));
            }
            
            

            // Reconsider the position of this block in the global list ( will affect who is loaded from disk / saved to disk or event flush out from memory
            void reorder( Block* b )
            {
                blocks.remove( b );
                blocks.insert( _find_pos( b ) , b );
                
                // Review if new free, write or reads are necessary
                _review();
                
            }

            // Experimental function ( only for debuggin that changes completelly the order of the blocks to get read / write / free )
            Block* getBlock( size_t id )
            {
                for ( std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; i++ )
                    if( (*i)->id == id )
                        return *i;
                return NULL;
            }
            
            
            std::string str()
            {
                std::ostringstream output;
                output <<"BLockManager: <Reads: " << num_reading_operations << " // Writes: " << num_writing_operations << " > [ " << au::Format::string( memory ) << " / " << au::Format::string(max_memory) << " ] " ;
                for (std::list<Block*>::iterator i = blocks.begin() ; i != blocks.end() ; )
                {
                    output << *(*i);
                    
                    i++;
                    if( i!=blocks.end() )
                        output << ",";
                }
                return output.str();
                
            }
            
        private:

            // Function to review pending read / free / write operations
            void _review();
            
            
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
            
            virtual void notify( engine::Notification* notification );
            
            
        private:
            
            void _freeMemoryWithLowerPriorityBLocks( Block *b );

            
        public:
            
            Info* getInfo();
            
            
        };
    }
}

#endif

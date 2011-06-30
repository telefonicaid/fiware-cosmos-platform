#ifndef _H_BLOCK_MATRIX
#define _H_BLOCK_MATRIX

/* ****************************************************************************
 *
 * FILE                      BlockMatrix.h
 *
 * AUTHOR                    Andreu Urruela Planas
 *
 * All the information related with a queue ( data and state )
 *
 */


#include <ostream>      // std::ostream
#include <string>       // std::string

#include "au/list.h"      // au::list
#include "au/map.h"         // au::map
#include "au/Cronometer.h"              // au::cronometer

#include "samson/common/coding.h"           // FullKVInfo

#include "samson/common/samson.pb.h"    // network::
#include "engine/Object.h"        // engine::Object

#define notification_review_task_for_queue "notification_review_task_for_queue"

namespace samson {
    
    namespace stream
    {
        class Block;
        class QueuesManager;

        class BlockList
        {
            friend class BlockMatrix;
            friend class Queue;
            friend class ParserQueueTask;
            friend class MapQueueTask;
            friend class ReduceQueueTask;
            
            // Blocks currently in the input queue
            au::list< Block > blocks;
            
            FullKVInfo accumulated_info;
            
        public:
            
            void add( Block *b );
            Block* extract( );
            
            // Get information
            size_t getSize();
            
            // String describing the stats of this
            std::string str();
            
            bool isEmpty();

            void retain();
            void release();
            
            void retain( size_t id );
            void release( size_t id );
            
            void lock();
            void unlock();
            
            bool isContentOnMemory();
            
            // Get information
            FullKVInfo getInfo();
            size_t getNumBlocks();
            
        private:
            
            au::list< Block >::iterator _find_pos( Block *b );
            
            
        };
        
        class BlockMatrix
        {
            
            // A list for each channel
            au::map<int , BlockList> channels;

            friend class ParserQueueTask;
            friend class MapQueueTask;
            friend class ReduceQueueTask;
            friend class Queue;
            
        public:
                      
            void add( int channel , Block *block );

            Block *extractFromChannel( int channel );
                        
            bool isEmpty();

            bool isEmpty( int channel_begin , int channel_end );
            
            
            // Some information
            std::string str();
            
            bool isContentOnMemory();
            
            void retain();
            void release();
            
            void retain( size_t id );
            void release( size_t id );
            
            void lock();
            void unlock();
            
            
            // Extract data
            void extract( BlockMatrix* _matrix , int channel_begin , int channel_end , size_t max_size );

            void extract( BlockMatrix* _matrix , int channel_begin , int channel_end )
            {
                // No limit in size
                extract( _matrix , channel_begin , channel_end , 0 );
            }
            
            // Copy all the blocks from another matrix
            void copyFrom( BlockMatrix* _matrix , int hg_begin , int hg_end );
          
            // Get information
            FullKVInfo getInfo();
            size_t getNumBlocks();
        };
        
    }
}

#endif

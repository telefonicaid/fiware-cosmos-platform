/* ****************************************************************************
 *
 * FILE                     BufferVector.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

/*
 
 BufferVector 
 
 */


#ifndef _H_BUFFER_VECTOR
#define _H_BUFFER_VECTOR

#include <cstring>			// size_t
#include <string>			// std::string
#include "au/map.h"			// au::map
#include "au/Lock.h"			// au::Lock
#include <vector>			// std::vector
#include "coding.h"			// ss::hg_info , ss::hg_size 
#include "engine/Buffer.h"					// ss::Buffer
#include "engine/MemoryManager.h"			// ss::MemoryManager
#include <set>						// std::set

namespace ss {
    
	/** 
     
	 Vector of buffers
     
     They are sorted usign the hg-set flag 
     
	 */
    
    
    class BufferVector
    {
        
    protected:
	
        au::list<engine::Buffer> buffers;	// List of sorted buffers 
        
    public:
        
        virtual ~BufferVector();
        
        // Add a paricular buffer to the vector following a particular order
        
        void addBuffer( engine::Buffer *b )
        {
            buffers.insert( _find_pos(b) , b );
        }

        size_t getTotalSize()
        {
            size_t size = 0;
            for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
                size += (*iter)->getSize();
            return size;
        }
        
        
        void clear();
        
    private:
        
        // Find the position to add a new buffer in the buffer list
        
        au::list<engine::Buffer>::iterator _find_pos( engine::Buffer *b )
        {
            for (std::list<engine::Buffer*>::iterator i = buffers.begin() ; i != buffers.end() ; i++)
            {
                if( (*i)->hg_set > b->hg_set )
                    return i;
            }
            
            return buffers.end();
        }
        
        
    };
    
    // Class to store a vector of buffers for a particular queue
    
	class QueueuBufferVector : public BufferVector
    {
        
    public:
        
		network::Queue* queue;		// Information about the queue we are buffering buffers to
        bool txt;                   // Type of buffer ( differnt aproach of generating new buffer )     
        
        // Constructor and desctructor
        QueueuBufferVector( const network::Queue & _queue , bool txt );
        QueueuBufferVector( QueueuBufferVector* qbv );
        ~QueueuBufferVector();
        
        // Function to add a particular buffer to the queue
        void addBuffer( network::WorkerDataExchange& workerDataExchange, engine::Buffer *b )
        {
            
            if ( !txt )
            {
                // Update the total ( size and number of kvs )
                KVHeader * header = (( KVHeader *) b->getData());
                
                // Assert magic number of incoming data packets
                if (!header->check())
                    LM_X(1, ("header check error"));
            }
            
            b->worker = workerDataExchange.worker();
            b->hg_set = workerDataExchange.hg_set();
            b->finish = workerDataExchange.finish();
            BufferVector::addBuffer( b );
        }

        // Get a new vector to be written on disk
        QueueuBufferVector* getQueueuBufferVectorToWrite()
        {
            if( txt )
                return getQueueuBufferVectorToWriteTXT();
            else
                return getQueueuBufferVectorToWriteKV();
        }
        
        // Create a new buffer from a consolidated vector of buffers
        engine::Buffer *getJoinedBuffer()
        {
            if( txt )
                return getJoinedBufferTXT();
            else
                return getJoinedBufferKV();
        }
        
        // Get information about status
		std::string getStatus();
        
        
    private:
        
        QueueuBufferVector*  getQueueuBufferVectorToWriteTXT();
        QueueuBufferVector*  getQueueuBufferVectorToWriteKV();

        engine::Buffer* getJoinedBufferTXT();
        engine::Buffer* getJoinedBufferKV();
        
    };

	
    
	
}

#endif

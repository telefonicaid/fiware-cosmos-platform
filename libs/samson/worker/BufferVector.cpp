
#include "engine/Buffer.h"					// samson::Buffer
#include "engine/MemoryManager.h"			// samson::MemoryManager
#include "BufferVector.h"                   // Own interface
#include "engine/Engine.h"                  // samson::Engine
#include "samson/common/SamsonSetup.h"                    // samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // MemoryOutputDisk

namespace samson {

    
#pragma mark BufferVector   
    
    BufferVector::~BufferVector()
    {
        // Clear the buffer if there is any buffer inside ( only in killed jobs )
        clearBufferVector();
    }

    
    void BufferVector::clearBufferVector()
    {
        for (std::list<engine::Buffer*>::iterator i = buffers.begin() ; i != buffers.end() ; i++)
            engine::MemoryManager::shared()->destroyBuffer( *i );
        buffers.clear();
    }

    
#pragma mark QueueBufferVector

    QueueuBufferVector::QueueuBufferVector( const network::Queue & _queue , bool _txt )
    {
        queue = new network::Queue();
        queue->CopyFrom( _queue );
        txt = _txt;
    }
    
    QueueuBufferVector::QueueuBufferVector( QueueuBufferVector* qbv )
    {
        queue = new network::Queue();
        queue->CopyFrom( *qbv->queue );
        txt = qbv->txt;
    }

    
    QueueuBufferVector::~QueueuBufferVector()
    {
        delete queue;
    }
    
    std::string QueueuBufferVector::getStatus()
    {
        std::ostringstream output;
        output << "Queue " << queue->name() << "";
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
            output << "[" << au::str( (*iter)->getSize() ,"B" ) << "]";
        return output.str();
    }
    
    
    QueueuBufferVector*  QueueuBufferVector::getQueueuBufferVectorToWriteTXT()
    {
        if( buffers.size() == 0 )
            return NULL;
        
		size_t file_size = 0;
        int current_hg_set = (*buffers.begin())->hg_set;

        au::list<engine::Buffer>::iterator first_iter = buffers.begin();
        au::list<engine::Buffer>::iterator last_iter = ++buffers.begin();
        
        for (  au::list<engine::Buffer>::iterator iter = ++buffers.begin() ; iter != buffers.end() ; iter++)
        {
            if( (*iter)->hg_set == current_hg_set )
            {
                if( (*iter)->finish )
                    current_hg_set++;
            }
            else if( (*iter)->hg_set > current_hg_set )
            {
                last_iter = iter;
                break;  // No not consider more elements
            }
            else 
                LM_X(1,("Internal samson error: Error sorting vector of buffers"));

			file_size += (*iter)->getSize();
        }
     
        size_t max_file_size = SamsonSetup::shared()->getUInt64("general.max_file_size");
        
        if( file_size > max_file_size )
        {
            QueueuBufferVector* qbv = new QueueuBufferVector( this );
            
            qbv->buffers.insert(qbv->buffers.begin(), buffers.begin() , last_iter );
            
            buffers.erase(buffers.begin() , last_iter);    // Note we do not remove the objects themselves
            
            return qbv;
        }
        
        return NULL; // Still no data to produce a new buffer
    }
    
    QueueuBufferVector*  QueueuBufferVector::getQueueuBufferVectorToWriteKV()
    {
		size_t file_size = 0;
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
			file_size += (*iter)->getSize();
        
        size_t max_file_size = SamsonSetup::shared()->getUInt64("general.max_file_size");
        
        if( file_size > max_file_size )
        {
            QueueuBufferVector* qbv = new QueueuBufferVector( this );
            
            for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++ )
                qbv->BufferVector::addBuffer( *iter );

            buffers.clear();    // Note we do not remove the objects themselves

            return qbv;
        }
        
        return NULL; // Still no data to produce a new buffer
    }

    
    engine::Buffer* QueueuBufferVector::getJoinedBufferTXT()
    {
		size_t file_size = 0;
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
			file_size += (*iter)->getSize();
        
        // Crearte the buffer
        engine::Buffer *b = engine::MemoryManager::shared()->newBuffer( "Creating txt file from buffers" , file_size , MemoryOutputDisk );
        
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
            b->write( (*iter)->getData(), (*iter)->getSize() );
        
        // Make sure buffer is correct
        if ( b->getSize() != b->getMaxSize())
            LM_X(1, ("size matters"));

        // Remove the used buffers
        clearBufferVector();
        
        return b;
    }

    engine::Buffer* QueueuBufferVector::getJoinedBufferKV()
    {
        // Compute the size of the future file
        size_t total_size = KVFILE_TOTAL_HEADER_SIZE;
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
            total_size += ( (*iter)->getSize() - KVFILE_TOTAL_HEADER_SIZE );
        
        // Format for the new queue
		KVFormat queue_format( queue->format().keyformat() , queue->format().valueformat() );
		
		// Get global information for all the buffers
		size_t global_size = 0;
        KVInfo global_info;
		
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
		{
            engine::Buffer *b = (*iter);
            
			KVHeader* header     = (KVHeader*) b->getData(); 
			KVInfo*  info	     = (KVInfo*)( b->getData() + sizeof( KVHeader ) );
            
			size_t    total_size = 0;
            
			for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
				total_size += info[hg].size;
            
            if (total_size != header->info.size)
                LM_X(1, ("total_size (%lu) != header->info.size (%lu)", total_size, header->info.size));
            
            global_size += total_size;
            global_info.append( header->info );
        }
        
        // Get a buffer to be able to put all data in memory
        size_t file_size = KVFILE_TOTAL_HEADER_SIZE + global_size;	
        
        // Crearte the buffer
        engine::Buffer *b = engine::MemoryManager::shared()->newBuffer( "Creating file from network buffers" , file_size , MemoryOutputDisk );
        
        // Global header of the file with magic number and format
        KVHeader fileHeader;
		fileHeader.init( queue_format , global_info );
		
		memcpy(b->getData(), &fileHeader, sizeof(KVHeader) );	
        
		// Vector with per-hash info
		KVInfo *file_info = (KVInfo*) (b->getData() + sizeof(fileHeader));
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = KVFILE_TOTAL_HEADER_SIZE;	// Initial offset at the file outptu
		
		// Init the offset in each file
        for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
			(*iter)->skipRead( KVFILE_TOTAL_HEADER_SIZE );
        
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
        {
            file_info[i].kvs = 0;
            file_info[i].size = 0;
            
            // Fill data and sizes from all buffers
            for (  au::list<engine::Buffer>::iterator iter = buffers.begin() ; iter != buffers.end() ; iter++)
            {
                KVInfo *_sub_info = (KVInfo*) ( (*iter)->getData() + sizeof( KVHeader ) );
                KVInfo sub_info = _sub_info[i];
                
                size_t read_size = (*iter)->read( data + offset , sub_info.size );
                
                if (read_size != sub_info.size)
                    LM_X(1, ("read_size (%d) != sub_info.size (%d)", read_size, sub_info.size));
                
                offset += sub_info.size;
                
                // Update information about this hash-group in the file buffer
                file_info[i].kvs += sub_info.kvs;
                file_info[i].size += sub_info.size;
            }
        }
		
		if (file_size != offset)
			LM_X(1, ("file_size (%d) != offset (%d)", file_size, offset));
        
        // Set the global size
        b->setSize(offset);
        
        // Remove the used buffers
        clearBufferVector();
        
        // Return the new buffer with the content reordered
        return b;      
    }
	
}
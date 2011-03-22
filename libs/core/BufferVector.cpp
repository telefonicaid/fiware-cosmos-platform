
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "BufferVector.h"			// Own interface
#include "Engine.h"			 // ss::Engine

namespace ss {

	QueueuBufferVector::QueueuBufferVector( const network::Queue &_queue , bool _txt )
	{
		// Init all the counter
		txt		= _txt;
		size	= 0;
		info.clear();
		
		// Get a copy of the network information
		queue = new network::Queue();
		queue->CopyFrom( _queue );
		
		clear();
	}

	QueueuBufferVector::~QueueuBufferVector()
	{
		delete queue;
	}
	
	void QueueuBufferVector::clear()
	{
		
		size = 0;
		info.clear();
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
		{
			Engine::shared()->memoryManager.destroyBuffer( buffer[i] );
			buffer[i] = NULL;
		}
		
		// Empty the vector of buffers
		buffer.clear();
		
	}
	
	void QueueuBufferVector::addBuffer( Buffer *b )
	{
		// Add the buffer to the vector
		buffer.push_back( b );
		
		if( txt )
			size += b->getSize(); 
		else
		{
			// Update the total ( size and number of kvs )
			KVHeader * header = (( KVHeader *) b->getData());
			
			// Assert magic number of incoming data packets
			if (!header->check())
				LM_X(1, ("header check error"));
			
			// Increase total information for this file
			info.kvs	+= header->info.kvs;
			info.size	+= header->info.size;

			// Increase the total size to monitorize the global file size
			size += header->info.size;
		}
	}
	
	Buffer* QueueuBufferVector::getJoinedBufferAndClear()
	{
		Buffer *outputBuffer;
		
		if( txt )
			outputBuffer = getTXTBufferFromBuffers();
		else
			outputBuffer = getFileBufferFromNetworkBuffers();
		
		clear();
		
		return outputBuffer;
	}
	
	/**
	 Buidl a TXT buffer with just the accumulation of the component buffers
	 */
	
	Buffer *QueueuBufferVector::getTXTBufferFromBuffers()
	{
		size_t file_size = 0;
		for (size_t i=0;i < buffer.size() ;i++)
			file_size += buffer[i]->getSize();
		
		// Crearte the buffer
		Buffer *b = Engine::shared()->memoryManager.newBuffer( "Creating txt file from buffers" , file_size , Buffer::output );
		
		for (size_t i=0;i < buffer.size() ;i++)
			b->write( buffer[i]->getData(), buffer[i]->getSize() );

		// Make sure buffer is correct
		if ( b->getSize() != b->getMaxSize())
			LM_X(1, ("size matters"));
		return b;
	}
	
	
	/**
	 Build a file in the SAMSON format with the incomming network buffers
	 */
	
	Buffer* QueueuBufferVector::getFileBufferFromNetworkBuffers( )
	{
		KVFormat queue_format( queue->format().keyformat() , queue->format().valueformat() );
		
		// Check all network buffers to be correct
		size_t global_size = 0;
		
		for (size_t i=0; i < buffer.size() ;i++)
		{
			KVHeader* header     = (KVHeader*) buffer[i]->getData(); 
			KVInfo*  info	     = (KVInfo*)( buffer[i]->getData() + sizeof( KVHeader ) );
			size_t    total_size = 0;

			for (int hg = 0 ; hg < KVFILE_NUM_HASHGROUPS ; hg++)
				total_size += info[hg].size;

			if (total_size != header->info.size)
				LM_X(1, ("total_size (%d) != header->info.size (%d)", total_size, header->info.size));

			global_size += total_size;
		}

		if (global_size != info.size)
			LM_X(1, ("global_size (%d) != info.size (%d)", global_size, info.size));
		
		// Get a buffer to be able to put all data in memory
		size_t file_size = KVFILE_TOTAL_HEADER_SIZE + info.size;	

		// Crearte the buffer
		Buffer *b = Engine::shared()->memoryManager.newBuffer( "Creating file from network buffers" , file_size , Buffer::output );
		
		// Global header of the file with magic number and format
		KVHeader fileHeader;
		fileHeader.init( queue_format , info );
		
		memcpy(b->getData(), &fileHeader, sizeof(KVHeader) );	

		// Vector with per-hash info
		KVInfo *file_info = (KVInfo*) (b->getData() + sizeof(fileHeader));
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = KVFILE_TOTAL_HEADER_SIZE;	// Initial offset at the file outptu
		
		// Init the offset in each file
		for (size_t i = 0 ; i < buffer.size() ; i++)
			buffer[i]->skipRead( KVFILE_TOTAL_HEADER_SIZE );
				
		for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
		{
			file_info[i].kvs = 0;
			file_info[i].size = 0;
			
			// Fill data and sizes from all buffers
			for (size_t j = 0 ; j <  buffer.size() ; j++)
			{
				KVInfo *_sub_info = (KVInfo*) ( buffer[j]->getData() + sizeof( KVHeader ) );
				KVInfo sub_info = _sub_info[i];

				size_t read_size = buffer[j]->read( data + offset , sub_info.size );
				
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
		
		// Return the new buffer with the content reordered
		return b;
		
	}
	
	
	std::string QueueuBufferVector::getStatus()
	{
		std::ostringstream o;
		o << "<" << buffer.size() << "/" << au::Format::string( size , "B" ) << ">";
		return o.str();
	}
	
	
}

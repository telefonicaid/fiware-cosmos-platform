
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "BufferVector.h"			// Own interface

namespace ss {

	QueueuBufferVector::QueueuBufferVector( network::Queue _queue , bool _txt )
	{
		queue = _queue;
		txt = _txt;

		info.kvs = 0;
		info.size = 0;
		
		size = 0;

	}
	
	void QueueuBufferVector::addBuffer( Buffer *b )
	{
		// Add the buffer to the vector
		buffer.push_back(b);
		
		if( txt )
			size += b->getSize(); 
		else
		{
			// Update the total ( size and number of kvs )
			NetworkHeader * header = (( NetworkHeader *) b->getData());
			
			// Increase total information for this file
			info.kvs += header->info.kvs;
			info.size += header->info.size;
			
			size += info.size;
		}
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
		Buffer *b = MemoryManager::shared()->newBuffer( "Creating txt file from buffers" , file_size );
		
		for (size_t i=0;i < buffer.size() ;i++)
			b->write( buffer[i]->getData(), buffer[i]->getSize() );

		// Make sure buffer is correct
		assert( b->getSize() == b->getMaxSize() );
	
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
			MemoryManager::shared()->destroyBuffer( buffer[i] );
		
		// Empty the vector of buffers
		buffer.clear();
		
		return b;
	}
	
	
	/**
	 Build a file in the SAMSON format with the incomming network buffers
	 */
	
	Buffer* QueueuBufferVector::getFileBufferFromNetworkBuffers( KVFormat queue_format )
	{
		
		// Check all network buffers to be correct
		size_t global_size=0;
		for (size_t i=0;i < buffer.size() ;i++)
		{
			NetworkHeader *header = (NetworkHeader*)buffer[i]->getData(); 
			NetworkKVInfo *info	  = (NetworkKVInfo*)( buffer[i]->getData() + sizeof( NetworkHeader ) );
			
			size_t total_size=0;
			for (int hg = 0 ; hg < KV_NUM_HASHGROUPS ; hg++)
				total_size += info[hg].size;
			assert( total_size == header->info.size );
			global_size += total_size;
		}
		assert( global_size == info.size );
		
		
		// Get a buffer to be able to put all data in memory
		size_t file_size = FILE_TOTAL_HEADER_SIZE + info.size;	

		// Crearte the buffer
		Buffer *b = MemoryManager::shared()->newBuffer( "Creating file from network buffers" , file_size );
		
		// Global header of the file with magic number and format
		FileHeader fileHeader;
		fileHeader.init( );
		fileHeader.setInfo( info );	
		fileHeader.setFormat( queue_format );
		memcpy(b->getData(), &fileHeader, sizeof(FileHeader) );	

		// Vector with per-hash info
		FileKVInfo *file_info = (FileKVInfo*) (b->getData() + sizeof(fileHeader));
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = FILE_TOTAL_HEADER_SIZE;	// Initial offset at the file outptu
		
		// Init the offset in each file
		for (size_t i = 0 ; i < buffer.size() ; i++)
			buffer[i]->skipRead( NETWORK_TOTAL_HEADER_SIZE );
				
		for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
		{
			file_info[i].kvs = 0;
			file_info[i].size = 0;
			
			// Fill data and sizes from all buffers
			for (size_t j = 0 ; j <  buffer.size() ; j++)
			{
				NetworkKVInfo *_sub_info = (NetworkKVInfo*) ( buffer[j]->getData() + sizeof( NetworkHeader ) );
				NetworkKVInfo sub_info = _sub_info[i];

				size_t read_size = buffer[j]->read( data + offset , sub_info.size );
				assert( read_size == sub_info.size);
				
				offset += sub_info.size;
				
				// Update information about this hash-group in the file buffer
				file_info[i].kvs += sub_info.kvs;
				file_info[i].size += sub_info.size;
			}
		}
		
		assert( file_size == offset );
		
		// Set the global size
		b->setSize(offset);
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
			MemoryManager::shared()->destroyBuffer( buffer[i] );
		
		// Empty the vector of buffers
		buffer.clear();
		
		// Reset accumulated size in this vector of buffers
		info.kvs = 0;
		info.size = 0;
		
		// Return the new buffer with the content reordered
		return b;
		
	}
	
	
	
}

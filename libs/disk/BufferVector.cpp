
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "BufferVector.h"			// Own interface

namespace ss {

	
	FileKVInfo BufferGetTotalInfo( Buffer *b )
	{
		return *((FileKVInfo*)b->getData());
	}
	
	NetworkKVInfo BufferGetInfo( Buffer *b , ss_hg hg )
	{
		NetworkKVInfo *tmp = (NetworkKVInfo*) b->getData();
		return tmp[hg+1];
	}
	
	
	BufferVector::BufferVector( )
	{
		info.kvs = 0;
		info.size = 0;
	}
	
	void BufferVector::addBuffer( Buffer *b )
	{
		// Update the total ( size and number of kvs )
		NetworkKVInfo * _info = (( NetworkKVInfo *) b->getData());
		
		// Increase total information for this file
		info.kvs += _info->kvs;
		info.size += _info->size;
		
		// Add the buffer to the vector
		buffer.push_back(b);
		
	}
	
	Buffer* BufferVector::getJoinedBuffer( )
	{
		 
		// Get a buffer to be able to put all data in memory
		size_t file_size = KV_HASH_GROUP_VECTOR_SIZE_FILE + info.size;	
		Buffer *b = MemoryManager::shared()->newBuffer( file_size );
		
		FileKVInfo *file_info = (FileKVInfo*) b->getData();
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = KV_HASH_GROUP_VECTOR_SIZE_FILE;	// Initial offset
		
		// Init the offset in each file
		for (size_t i = 0 ; i < buffer.size() ; i++)
			buffer[i]->skipRead( KV_HASH_GROUP_VECTOR_SIZE_NETWORK );
		
		// Global info
		file_info->size	= info.size;
		file_info->kvs	= info.kvs;
		
		for (size_t i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
		{
			file_info[i+1].kvs = 0;
			file_info[i+1].size = 0;
			
			// Fill data and sizes from all buffers
			for (size_t j = 0 ; j <  buffer.size() ; j++)
			{
				NetworkKVInfo sub_info = BufferGetInfo( buffer[j] , i );
				
				buffer[j]->read( data + offset , sub_info.size );
				offset += sub_info.size;
				
				file_info[i+1].kvs += sub_info.kvs;
				file_info[i+1].size += sub_info.size;
			}
		}
		
		// Set the global size
		b->setSize(offset);
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
			MemoryManager::shared()->destroyBuffer( buffer[i] );
		buffer.clear();
		
		// Reset accumulated size in this vector of buffers
		info.kvs = 0;
		info.size = 0;
		
		// Return the new buffer with the content reordered
		return b;
		
	}
	
	
	
}


#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "BufferVector.h"			// Own interface

namespace ss {

	
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
		assert( false );
		// TODO: To be reviews with the Buffer updated
		
		/*
		 
		// Get a buffer to be able to put all data in memory
		size_t file_size = KV_HASH_GROUP_VECTOR_SIZE + info.size;	
		Buffer *b = MemoryManager::shared()->newBuffer( file_size );
		
		
		hg_info *info_buffer = (hg_info*) b->getData();
		
		// Global data and offset in the resulting buffer
		char *data = b->getData();
		size_t offset = KV_HASH_GROUP_VECTOR_SIZE;
		
		// Init the offset in each file
		for (size_t i = 0 ; i < buffer.size() ; i++)
			buffer[i]->size = KV_HASH_GROUP_VECTOR_SIZE;
		
		info_buffer[0] = info;
		for (size_t i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
		{
			info_buffer[i+1].kvs = 0;
			info_buffer[i+1].size = 0;
			
			// Fill data and sizes from all buffers
			for (size_t j = 0 ; j <  buffer.size() ; j++)
			{
				
				hg_info sub_info = BufferGetInfo( buffer[j] , i );
				
				buffer[j]->write( data + offset , sub_info.size );
				memcpy(data + offset, buffer[j]->getCurrentDataAndUpdateOffset( sub_info.size ) , sub_info.size);
				offset += sub_info.size;
				
				info_buffer[i+1].kvs ++;
				info_buffer[i+1].size += sub_info.size;
				
			}
			
		}
		
		// Remove all buffers
		for (size_t i = 0 ; i < buffer.size() ; i++)
			MemoryManager::shared()->destroyBuffer( buffer[i] );
		buffer.clear();
		
		// Reset accumulated size in this vector of buffers
		info.kvs = 0;
		info.size = 0;
		
		// Return the new buffer with the content reordered
		return b;
		 */
		
		return NULL;
	}
	
	
	
}

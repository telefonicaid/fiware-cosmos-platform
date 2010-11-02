
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager

namespace ss {
	
#pragma mark BufferVector
	
	BufferVector::BufferVector( size_t _task_id , std::string _queue)
	{
		task_id = _task_id;
		queue = _queue;
		
		info.kvs = 0;
		info.size = 0;
	}
	
	void BufferVector::addBuffer( Buffer *b )
	{
		
		
		// Update the total ( size and number of kvs )
		hg_info * _info = (( hg_info *) b->getData());
		
		// Flush first if if will get a larger file
		if( (info.size + _info->size) > KV_MAX_FILE_SIZE )
			flushToDisk();
		
		// Increase total and add vector
		info.kvs += _info->kvs;
		info.size += _info->size;
		push_back(b);
		
		
	}
	
	void BufferVector::flushToDisk( )
	{
		
		// Get a buffer to be able to put all data in memory
		size_t file_size = KV_HASH_GROUP_VECTOR_SIZE + info.size;	
		Buffer *buffer = MemoryManager::shared()->newBuffer( file_size );
		
		// TODO: To be completed...		
		hg_info *info_buffer = (hg_info*) buffer->getData();
		
		info_buffer[0] = info;
		for (size_t i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
		{
			info_buffer[i+1].kvs = 0;
			info_buffer[i+1].size = 0;

			// Fill data and sizes
			
		}
		
		// Send to save to disk
		// TODO: To be completed
		
		// Flush to disk for this queue ( removing buffers used )
		for (size_t i = 0 ; i < size() ; i++)
			MemoryManager::shared()->destroyBuffer( (*this)[i] );
		clear();
	}
	
	
#pragma mark DataBuffer
	
	void DataBuffer::addBuffer( size_t task_id , std::string queue , Buffer* buffer )
	{
		lock.lock();
		
		
		TaskDataBuffer *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			tdb = new TaskDataBuffer( task_id );
			insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer );
		
		lock.unlock();
	}
	
	
	void DataBuffer::closeTask( size_t task_id )
	{
		lock.lock();
		
		TaskDataBuffer *t = extractFromMap( task_id );
		
		if( t )
		{
			t->flushToDisk( task_id );
			delete t;
		}

		lock.unlock();
		
	}
	
	
}


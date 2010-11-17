
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// ss::DataBufferItem
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate

namespace ss {
	
#pragma mark Use Buffer to get information
	
	
	FileKVInfo BufferGetTotalInfo( Buffer *b )
	{
		return *((FileKVInfo*)b->getData());
	}

	NetworkKVInfo BufferGetInfo( Buffer *b , ss_hg hg )
	{
		NetworkKVInfo *tmp = (NetworkKVInfo*) b->getData();
		return tmp[hg+1];
	}
	
	std::string BufferToString( Buffer *b )
	{
		std::ostringstream info;
		NetworkKVInfo * _info = (ss::NetworkKVInfo*) b->getData(); 
		info << "Buffer with " << au::Format::string(_info[0].kvs) << " kvs in " << au::Format::string(_info[0].size) << "bytes (";
		/*
		for (int i = 0 ; i < 20 ; i++ )
			info << _info[i+1].kvs << "/" << _info[i+1].size << " ";
		*/
		return info.str();
	}
	
	void DataBuffer::addBuffer( size_t task_id , std::string queue , Buffer* buffer )
	{
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( task_id );
			insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer );
		
		lock.unlock();
	}
	
	
	void DataBuffer::finishTask( size_t task_id )
	{
		lock.lock();
		
		DataBufferItem *item = findInMap( task_id );
		
		if( item )
			item->finish();			// Notify to the item that it is finish to flush last file to disk
		
		lock.unlock();
		
	}
	
	void DataBuffer::removeTask( size_t task_id )
	{
		lock.lock();

		DataBufferItem *tdb = extractFromMap( task_id );
		if( tdb )
			delete tdb;
		
		lock.unlock();
	}
	
	
}


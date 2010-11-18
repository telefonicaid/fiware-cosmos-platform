
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// ss::DataBufferItem
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate
#include "SamsonWorker.h"			// ss::SamsonWorker

namespace ss {
	
#pragma mark Use Buffer to get information

	
	
	
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

	DataBuffer::DataBuffer( SamsonWorker *_worker )
	{
		worker = _worker;
	}
	
	
	void DataBuffer::addBuffer( size_t task_id , std::string queue , Buffer* buffer )
	{
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getNumWorkers() );
			insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer );
		
		lock.unlock();
	}
	
	
	void DataBuffer::finishWorker( size_t task_id )
	{
		lock.lock();
		
		DataBufferItem *item = findInMap( task_id );
		
		if( item )
			item->finishWorker();			// Notify to the item that it is finish to flush last file to disk
		
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

	std::string DataBuffer::getStatus()
	{
		lock.lock();
		std::string txt =  getStatusFromArray( *this );
		lock.unlock();
		
		return txt;
	}
	
	
}


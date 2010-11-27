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
	
	
	void DataBuffer::addBuffer( size_t task_id , network::Queue queue , Buffer* buffer )
	{
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers() );
			insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer );
		
		lock.unlock();
	}
	
	
	void DataBuffer::finishWorker( size_t task_id )
	{
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers() );
			insertInMap( task_id , tdb );
		}
		
		tdb->finishWorker();			// Notify to the item that it is finish to flush last file to disk
	
		// If there is nothing to write, right now is completed
		
		if ( tdb->isCompleted() )
		{
			
			DataBufferItem *tdb = extractFromMap( task_id );
			delete tdb;
		}
		
		
		lock.unlock();

		
	}
	

	std::string DataBuffer::getStatus()
	{
		lock.lock();
		std::string txt =  getStatusFromArray( *this );
		lock.unlock();
		
		return txt;
	}
	
	void DataBuffer::diskManagerNotifyFinish(size_t id, bool success)
	{
		lock.lock();
		
		// Discover the DataBufferItem with id_relation
		std::map<size_t,size_t>::iterator iter = id_relation.find( id );
		if( iter != id_relation.end() )
		{
			
			size_t task_id = iter->second;
			
			
			DataBufferItem *tdb = findInMap( task_id );
			tdb->diskManagerNotifyFinish(id, success);
			
			if ( tdb->isCompleted() )
			{
				DataBufferItem *tdb = extractFromMap( task_id );
				delete tdb;
			}
			
			
		}
		else {
			assert(false);
		}

		
		lock.unlock();
		
	}
	
	
	
	
}


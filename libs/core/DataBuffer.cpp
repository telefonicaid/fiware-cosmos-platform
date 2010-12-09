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

	DataBuffer::DataBuffer( SamsonWorker *_worker )
	{
		worker = _worker;
		setStatusTile( "Data Buffer" , "db" );
	}
		
	void DataBuffer::addBuffer( size_t task_id , network::Queue queue , Buffer* buffer, bool txt )
	{
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers()  );
			insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer , txt );
		
		lock.unlock();
	}
	
	
	void DataBuffer::finishWorker( size_t task_id )
	{
		
		lock.lock();
		
		DataBufferItem *tdb = findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers()  );
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

	void DataBuffer::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";
		getStatusFromMap( output , *this , prefix_per_line );
	}


	
	void DataBuffer::fileManagerNotifyFinish(size_t id, bool success)
	{
		
		lock.lock();
		
		if( id_relation.isInMap( id ) )
		{
			size_t task_id = id_relation.extractFromMap( id );
			
			DataBufferItem *tdb = findInMap( task_id );

			assert( tdb );
			
			tdb->fileManagerNotifyFinish(id, success);
			
			if ( tdb->isCompleted() )
			{
				DataBufferItem *tdb = extractFromMap( task_id );
				delete tdb;
			}
			
		}
		else {
			assert(false);
			// There is something wrong.
			// There is no relation between this FileManager id and any task
		}
		
		lock.unlock();
		
	}
	
	
	
	
}


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
		
		DataBufferItem *tdb = item.findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers()  );
			item.insertInMap( task_id , tdb );
		}
		
		tdb->addBuffer( queue , buffer , txt );
		
		lock.unlock();
	}
	
	
	void DataBuffer::finishWorker( size_t task_id )
	{
		lock.lock();
		
		DataBufferItem *tdb = item.findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getWorkerId(), worker->network->getNumWorkers()  );
			item.insertInMap( task_id , tdb );
		}
		
		tdb->finishWorker();
	
		if ( tdb->isCompleted() )
		{
			
			// Notify to the task manager that this is completed
			worker->taskManager.completeTask( task_id );	

			// Extract and remove task
			DataBufferItem *tdb = item.extractFromMap( task_id );
			delete tdb;
		}
		
		lock.unlock();
		
	}
	
	void DataBuffer::fileManagerNotifyFinish(size_t id, bool success)
	{
		
		lock.lock();
		
		if( id_relation.isInMap( id ) )
		{
			size_t task_id = id_relation.extractFromMap( id );
			
			DataBufferItem *tdb = item.findInMap( task_id );

			assert( tdb );
			
			tdb->fileManagerNotifyFinish(id, success);
			
			if ( tdb->isCompleted() )
			{
				// Notify to the task manager that this is completed
				worker->taskManager.completeTask( task_id );	
				
				DataBufferItem *tdb = item.extractFromMap( task_id );
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
	
	
	
	
	void DataBuffer::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";
		getStatusFromMap( output , item , prefix_per_line );
	}
	
	void DataBuffer::fill(network::WorkerStatus*  ws)
	{
		std::ostringstream output;
		output << item.size() << " active elements";
		ws->set_data_buffer_status( output.str() );
	}
	
	
}


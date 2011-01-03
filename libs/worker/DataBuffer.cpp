#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// ss::DataBufferItem
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "BufferVector.h"		// ss:QueueuBufferVector
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"

namespace ss {
	
	
#pragma mark thread function
	
	void* _runBackgroundThreadToProcessBufferVectors(void*p)
	{
		((DataBuffer *)p)->runBackgroundThreadToProcessBufferVectors();
		return NULL;
	}
	
#pragma mark Use Buffer to get information

	DataBuffer::DataBuffer( SamsonWorker *_worker ) : stopLock( &lock )
	{
		worker = _worker;
		setStatusTile( "Data Buffer" , "db" );
		
		// Create the thread to process buffer vectors in backgroudn
		pthread_t t;
		pthread_create(&t, NULL, _runBackgroundThreadToProcessBufferVectors, this);
	}
		
	void DataBuffer::addBuffer( size_t task_id , network::Queue queue , Buffer* buffer, bool txt )
	{
		lock.lock();
		
		DataBufferItem *tdb = item.findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item if necessary
			tdb =  new DataBufferItem( this, task_id , worker->network->getNumWorkers()  );
			item.insertInMap( task_id , tdb );
		}
		
		bool wake_up_background_thread = tdb->addBuffer( queue , buffer , txt );
		
		lock.unlock();
		
		if( wake_up_background_thread )
			lock.wakeUpStopLock(&stopLock);
		
	}
	
	
	void DataBuffer::finishWorker( size_t task_id )
	{
		
		lock.lock();
		
		DataBufferItem *tdb = item.findInMap( task_id );
		
		if (!tdb )
		{
			// Create a new item
			tdb =  new DataBufferItem( this, task_id , worker->network->getNumWorkers()  );
			item.insertInMap( task_id , tdb );
		}

		// Notify about the finish worker message
		bool wake_up_background_thread = tdb->finishWorker();
	
		// Extract and remove task if necessary
		if ( tdb->to_be_removed )
		{
			DataBufferItem *tdb = item.extractFromMap( task_id );
			delete tdb;
		}
		
		lock.unlock();

		if( wake_up_background_thread )
			lock.wakeUpStopLock(&stopLock);

		
		
	}
	
	void DataBuffer::fileManagerNotifyFinish(size_t id, bool success)
	{
		lock.lock();

		// It is here sure because we have added with the background thread
		assert( id_relation.isInMap( id ) );

		// Get the task_id associated to this file
		size_t task_id = id_relation.extractFromMap( id );
		
		// Get the DataBufferItem for this task
		DataBufferItem *tdb = item.findInMap( task_id );
		assert( tdb );

		// Notify about this new file
		tdb->fileManagerNotifyFinish(id, success);
		
		// Extract and remove task if necessary
		if ( tdb->to_be_removed )
		{
			DataBufferItem *tdb = item.extractFromMap( task_id );
			delete tdb;
		}		
		
		lock.unlock();
		
	}
	
	
	void DataBuffer::runBackgroundThreadToProcessBufferVectors()
	{
		while( true )
		{
			
			lock.lock();	// Lock to search for the next Buffer vector
			
			
			if( pendingBufferVectors.size() > 0 )
			{
				QueueuBufferVector * bv = pendingBufferVectors.front();
				pendingBufferVectors.pop_front();
			
				lock.unlock();
				
				// Create the new joined buffer
				// ---------------------------------------------------------------------------
				Buffer *b = bv->getJoinedBufferAndClear();
				assert(b);
				

				std::string fileName = _newFileName( bv->queue->name() );
				
				// Notify the controller that a file has been created ( update )
				// ---------------------------------------------------------------------------
				
				network::QueueFile qf;
				qf.set_queue( bv->queue->name() );
				network::File *file = qf.mutable_file();
				file->set_name( fileName );
				file->set_worker( worker->network->getWorkerId() );
				network::KVInfo *info = file->mutable_info();
				
				// This is suppoused to be a file ( txt or kv )
				if( bv->txt )
				{
					info->set_size( b->getSize() );
					info->set_kvs( 1 );
				}
				else
				{
					FileHeader * header = (FileHeader*) ( b->getData() );
					info->set_size( header->info.size);
					info->set_kvs(header->info.kvs);
				}
				
				// Send a packet to the controller informing about this new file
				WorkerTaskManager::send_add_file_message_to_controller( worker->network , bv->task_id , qf);
				
				
				// Schedule at the File Manager ( Note that delegate id DataBuffer )
				// ----------------------------------------------------------------------
				
				lock.lock();
				
				size_t id = FileManager::shared()->addItemToWrite( new FileManagerWriteItem( fileName, b , this ) ) ;
				id_relation.insert( std::pair<size_t,size_t>( id, bv->task_id) );
				
				
				// Inform to the item that this file is created
				DataBufferItem *_item = item.findInMap( bv->task_id );
				assert( _item );
				_item->newFileCreated();
				
				// Delete the Buffer vector
				delete bv;
				
				lock.unlock();
				
			}
			else
				lock.unlock_waiting_in_stopLock( &stopLock );
		}
	}
	
	void DataBuffer::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";
		getStatusFromMap( output , item , prefix_per_line );
	}
	
	void DataBuffer::fill(network::WorkerStatus*  ws)
	{
		std::ostringstream output;
		
		lock.lock();
		
		std::map<size_t , DataBufferItem*>::iterator iter;
		for (iter = item.begin(); iter != item.end() ; iter++)
			output << "[" << iter->second->getStatus() << "]";
		
		lock.unlock();
		
		ws->set_data_buffer_status( output.str() );
	}
	
	std::string DataBuffer::_newFileName( std::string queue )
	{
		std::ostringstream fileName;
		fileName << SAMSON_DATA_DIRECTORY << "file_" << queue << "_" << rand()%10000 << rand()%10000 << rand()%10000;
		return fileName.str();
	}
	
	
	
	
	
	
	
}


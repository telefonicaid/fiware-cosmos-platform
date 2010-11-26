
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// Own interface
#include "BufferVector.h"			// ss::BufferVector
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate
#include "DataBuffer.h"				// ss::DataBuffer
#include "SamsonWorker.h"			// ss::SamsonWorker

namespace ss {
	
	DataBufferItem::DataBufferItem( DataBuffer *_dataBuffer , size_t _task_id , int _myWorkerId, int _num_workers )
	{
		dataBuffer = _dataBuffer;
		task_id = _task_id;

		num_finished_workers = 0;
		
		num_workers = _num_workers;
		myWorkerId = _myWorkerId;
		
		finished = false;	// Flag
		completed = false;
		
		
	}	
	
	void DataBufferItem::addBuffer( network::Queue queue , Buffer *buffer )
	{
		std::string name = queue.name();
		QueueuBufferVector* bv = findInMap( name );
		
		if( !bv )
		{
			bv = new QueueuBufferVector( queue );
			insertInMap( name , bv  );
		}
		
		if( buffer->getSize() + bv->getSize() > KV_MAX_FILE_SIZE )
		{
			Buffer *b = bv->getFileBufferFromNetworkBuffers( KVFormat( queue.format().keyformat() , queue.format().valueformat() ) );
			std::string fileName = newFileName( name );
			saveBufferToDisk( b , fileName , bv->queue );
		}
		
		bv->addBuffer( buffer );
		
	}
	
	void DataBufferItem::finishWorker()
	{
		num_finished_workers++;
		
		if( num_finished_workers == num_workers )
		{
			// Set the finish flag to indicate that this task operation is finished
			finished = true;
		
			// Flush to disk the rest of the buffer
			for ( std::map<std::string , QueueuBufferVector* >::iterator i = begin() ; i != end() ; i++)
			{
				if( i->second->getSize() > 0)
				{
					QueueuBufferVector *bv = i->second;
					network::Queue queue = bv->queue;
					Buffer *b = bv->getFileBufferFromNetworkBuffers( KVFormat( queue.format().keyformat() , queue.format().valueformat() ) );
					std::string fileName = newFileName( i->first );
					saveBufferToDisk( b , fileName , bv->queue );
				}
			}

			// Just in case, there is nothing else to save
			if( ids_files.size() == 0 )
			{
				completed = true;
				
				// Notify to the task manager that this is completed
				dataBuffer->worker->taskManager.completeTask( task_id ,  this );
				
				DataBufferItem *tdb = dataBuffer->extractFromMap( task_id );
				if( tdb )
					delete tdb;
				
				return;	// No necessary unlock since we do not exist any more ;)
			}
		}		
	}	
	
	void DataBufferItem::saveBufferToDisk( Buffer* b , std::string fileName , network::Queue queue )
	{
		// Store this file to be notified later to the controller
		network::QueueFile qf;
		qf.set_queue( queue.name() );
		
		network::File *file = qf.mutable_file();
		file->set_name( fileName );
		file->set_worker( myWorkerId );

		network::KVInfo *info = file->mutable_info();
		
		// this is suppoused to be a file
		FileKVInfo * _info = (FileKVInfo*) ( b->getData() + sizeof(FileHeader) );
		info->set_size(_info->size);
		info->set_kvs(_info->kvs);
		
		qfiles.push_back( qf );
		
		size_t id = FileManager::shared()->write( b , fileName, this );
		
		// Keep ids to control if files are saved on disk
		lock.lock();
		ids_files.insert( id );
		lock.unlock();
	}
	
	void DataBufferItem::diskManagerNotifyFinish(size_t id, bool success)
	{
		lock.lock();
		
		if( success )
			ids_files_saved.insert( id );
		else
		{
			assert( false );
		}
		
		if( !completed && ( ids_files.size() == ids_files_saved.size() ) && finished )
		{
			completed = true;

			// Notify to the task manager that this is completed
			dataBuffer->worker->taskManager.completeTask( task_id ,  this );

			DataBufferItem *tdb = dataBuffer->extractFromMap( task_id );
			if( tdb )
				delete tdb;

			return;	// No necessary unlock since we do not exist any more ;)
			
			
		}
		
		lock.unlock();
		
	}	
	
	std::string DataBufferItem::newFileName( std::string queue)
	{
		std::ostringstream fileName;
		fileName << "/tmp/" << queue << rand()%1000 << rand()%1000;
		return fileName.str();
	}
	
	std::string DataBufferItem::getStatus()
	{
		std::ostringstream output;
		if( completed )
			output << "[COMPLETED]";
		else if( finished )
			output << "[FINISHED] ";
		
		output << "[Closed " << num_finished_workers << " of " << num_workers << " workers ] ";
		output << "[Files ids " << ids_files_saved.size() << " / " << ids_files.size() << " files ] ";
		output << "[Created " << qfiles.size() << " files ] ";
		
		std::map<std::string , QueueuBufferVector* >::iterator i;
		for (i = begin() ; i != end() ; i++)
			output << "[ Queue: " << i->first << " " << i->second->getInfo().str() << " ]";
		
		return output.str();
	}	
	
	
}

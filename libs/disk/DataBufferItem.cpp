
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// Own interface
#include "BufferVector.h"			// ss::BufferVector
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate

namespace ss {
	
	DataBufferItem::DataBufferItem( size_t _task_id , DataBufferItemDelegate * _delegate , std::vector<std::string> queues )
	{
		task_id = _task_id;
		delegate = _delegate;
		
		finished = false;	// Flag
		
		// Create a vector of Buffers for each queue
		for ( size_t i = 0 ; i < queues.size() ;i++)
			insertInMap( queues[i] , new BufferVector() );
		
	}	
	
	void DataBufferItem::addBuffer( std::string queue , Buffer *buffer )
	{
		BufferVector* bv = findInMap( queue );
		
		if( !bv )
		{
			// Some error
			MemoryManager::shared()->destroyBuffer( buffer );
			return ;
		}
		
		if( buffer->getSize() + bv->getSize() > KV_MAX_FILE_SIZE )
		{
			Buffer *b = bv->getJoinedBuffer( );
			std::string fileName = newFileName( queue );
			saveBufferToDisk( b , fileName , queue );
		}
		
		bv->addBuffer( buffer );
		
	}
	
	void DataBufferItem::finish()
	{
		// Flush to disk the rest of the buffer
		
		for ( std::map<std::string , BufferVector* >::iterator i = begin() ; i != end() ; i++)
		{
			if( i->second->getSize() > 0)
			{
				BufferVector *bv = i->second;
				
				Buffer *b = bv->getJoinedBuffer( );
				std::string fileName = newFileName( i->first );
				
				saveBufferToDisk( b , fileName , i->first );
			}
		}
		
		// Set the finish flag to indicate that this task operation is finished
		finished = true;
		
		if( ids_files.size() == 0)
		{
			// Notify the delegate it's over
			delegate->finishDataBufferTask(task_id);
			
		}
		
	}	
	
	void DataBufferItem::saveBufferToDisk( Buffer* b , std::string fileName , std::string queue )
	{
		// Notify the delegate about this new file
		delegate->addFile( task_id, fileName , queue , BufferGetTotalInfo(b) );
		
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
			ids_files.erase( id );
		
		if( ids_files.size() == 0 && finished )
		{
			delegate->finishDataBufferTask(task_id);
		}
		lock.unlock();
		
	}	
	
	std::string DataBufferItem::newFileName( std::string queue)
	{
		std::ostringstream fileName;
		fileName << "/tmp/" << queue << rand()%1000 << rand()%1000;
		return fileName.str();
	}
	
	
	
}
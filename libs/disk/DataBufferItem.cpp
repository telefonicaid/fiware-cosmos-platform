
#include "DataBuffer.h"				// Own interface
#include "Buffer.h"					// ss::Buffer
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "FileManager.h"			// ss::FileManager	
#include "DataBufferItem.h"			// Own interface
#include "BufferVector.h"			// ss::BufferVector
#include "DataBufferItemDelegate.h"	// ss::DataBufferItemDelegate

namespace ss {
	
	DataBufferItem::DataBufferItem( size_t _task_id )
	{
		task_id = _task_id;
		
		finished = false;	// Flag
		
		
	}	
	
	void DataBufferItem::addBuffer( std::string queue , Buffer *buffer )
	{
		BufferVector* bv = findInMap( queue );
		
		if( !bv )
		{
			bv = new BufferVector();
			insertInMap( queue , bv  );
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
			// Notify controller that all files are saved to disk
			// TODO: Pending stuff
			
		}
		
	}	
	
	void DataBufferItem::saveBufferToDisk( Buffer* b , std::string fileName , std::string queue )
	{
		// Store this file to be notified latter to the controller
		network::File f;
		f.set_name( fileName );
		files.push_back( f );
		
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
			// TODO: Do something with this
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
		if( finished )
			output << "[FINISHED] ";
		output << "[Pending to be confirmed " << ids_files.size() << " files ] ";
		output << "[Created " << files.size() << " files ] ";
		
		std::map<std::string , BufferVector* >::iterator i;
		for (i = begin() ; i != end() ; i++)
			output << "[ Queue: " << i->first << " " << i->second->getInfo().str() << " ]";
		
		return output.str();
	}	
	
	
}


#include "FileManager.h"			// Own interface
#include "FileManagerItem.h"
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"


namespace ss
{

	
	static FileManager* sharedFileManager=NULL;
	
	FileManager* FileManager::shared()
	{
		if( !  sharedFileManager )
			sharedFileManager = new FileManager();
		return sharedFileManager;
	}

	
	
	size_t FileManager::addItemToRead( FileManagerReadItem* v )
	{
		lock.lock();
		
		// Set and id to the new element
		size_t fm_id = current_fm_id++;
		v->setId(fm_id);
		
		// add the the internal map
		items.insertInMap( fm_id , v);
		
		if( !cacheSystem.addReadItem( v ) )	// If not possible to read from cache
		{
			
			// Add to the disk manager
			size_t dm_id = DiskManager::shared()->read( v->simpleBuffer.getData() , v->fileName , v->offset , v->size,  this );
			
			// add the relation between both ids
			ids.insertInMap( dm_id , fm_id );
		}
		
		lock.unlock();
		
		return fm_id;
		
	};
	
	size_t FileManager::addItemToWrite( FileManagerWriteItem* v )
	{
		lock.lock();
		
		size_t fm_id = current_fm_id++;
		v->setId(fm_id);
		
		items.insertInMap( fm_id , v );
		
		size_t dm_id = DiskManager::shared()->write( v->buffer , v->fileName , this );
		
		// add the relation between both ids
		ids.insertInMap( dm_id , fm_id );
		
		// Add into the cache system
		cacheSystem.addWriteItem(  v );
		
		lock.unlock();
		
		return fm_id;
	};
	
	
	
	/**
	 Delegate interface of DiskManager
	 It is notified when a write / read is finished
	 */
	
	void FileManager::diskManagerNotifyFinish( size_t id, bool success)
	{
		
		lock.lock();
		
		size_t dm_id = id;
		
		if( ids.isInMap( dm_id ) )
		{
			size_t fm_id = ids.extractFromMap( dm_id );
			lock.unlock();
			
			finishItem( fm_id , success );
			return;
			
		}
		else
		{
			lock.unlock();
			assert(false);// No idea what to do...
		}
		
		
	}
	
	
	void FileManager::finishItem( size_t fm_id , bool success )
	{
		FileManagerDelegate *delegate = NULL;
		
		lock.lock();
		
		FileManagerItem *item = items.extractFromMap( fm_id );
		
		if( item )
		{
			
			item->addStatistics( &statistics );
			
			if( item->type == FileManagerItem::write )
			{
				// Add into the cache system
				cacheSystem.notifyWriteItemFinished( (FileManagerWriteItem*) item );
			}
			
			delegate = item->delegate;
			delete item;							// we are responsible for deleting this 
		}
		
		lock.unlock();
		
		// Call the delegate outside the lock to avoid dead-lock
		assert(delegate);	// It is not allowed non-delegate calls
		
		if( delegate )
			delegate->fileManagerNotifyFinish( fm_id , success); 
		
	}	
	
	
	void FileManager::fill(network::WorkerStatus*  ws)
	{
		std::ostringstream output;
		lock.lock();
		output << items.size() << " items : ";
		output << "Statistics: " << statistics.getStatus();
		lock.unlock();
		
		ws->set_file_manager_status( output.str() );
		ws->set_file_manager_cache_status( cacheSystem.getStatus() );
		
	}
	
	
	
	
	
}
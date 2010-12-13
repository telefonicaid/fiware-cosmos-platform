

#include "FileManagerCacheSystem.h"		// Own interface
#include "FileManagerCacheItem.h"
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"
#include "FileManagerCacheTask.h"
#include "FileManager.h"					// ss::FileManager
#include "FileManagerCacheItem.h"			// ss::FileManagerCacheItem

namespace ss {

	void* runFileManagerCacheSystemThread(void*p)
	{
		FileManagerCacheSystem *cacheSystem = (FileManagerCacheSystem*)p;
		cacheSystem->run();
		return NULL;
	}


	void FileManagerCacheSystem::run()
	{
		// Thread main run
		while( true )
		{
			
			// Free memory is necessary and possible
			freeMemoryIfPossible(0.7);
			
			lock.lock();
			FileManagerCacheTask *task = NULL;
			
			if( tasks.size() > 0 )
			{
				task = tasks.front();
				tasks.pop_front();
				lock.unlock();
				
				// Perform task
				if( task )
				{
					memcpy(task->readItem->buffer, task->cacheItem->buffer->getData() + task->readItem->offset , task->readItem->size);
					
					lock.lock();
					task->cacheItem->release();
					lock.unlock();
					
					// Nofity this is ready
					FileManager::shared()->finishItem( task->readItem->getId() , true );
					
					delete task;
				}
				
			}
			else
				lock.unlock_waiting_in_stopLock( &stopLock , 5 );	// Timeout to free memory if necessary every 5 seconds
		}
	}

	FileManagerCacheSystem::FileManagerCacheSystem() : stopLock( &lock )
	{
		// Reset size 
		size_in_cache = 0;
		num_files_in_cache = 0;
		
		// Create the thread that reads elements from cache in background
		pthread_create(&t, NULL, runFileManagerCacheSystemThread, this);
	}
	
	void FileManagerCacheSystem::freeMemoryIfPossible( double memory_per)
	{
		
		while( MemoryManager::shared()->getMemoryUsage() >= memory_per )
		{
			
			bool answer = false;
			
			lock.lock();
			
			FileManagerCacheItem *item = cache.extractFromBack();
			
			if( item )
			{
				if( !item->isUsed() )
				{
					size_in_cache -= item->buffer->getSize();
					num_files_in_cache--;
					
					item->destroy();
					answer = true;
					delete item;
				}
				else
				{
					// Insert again the cache system
					cache.insertAtBack( item->fileName , item );
				}
			}
			
			lock.unlock();
			
			if( !answer )
				return;
		}
		
	}
	
	/**
	 Add a request for a file
	 Return true, if it is in the cache and can be served
	 */
	
	bool FileManagerCacheSystem::addReadItem( FileManagerReadItem *read_item )
	{
		bool answer = false;
		
		lock.lock();
		
		FileManagerCacheItem *item = cache.extractFromMap( read_item->fileName );
		
		if( item )
		{
			answer = true;		// Yes, it can be served from cache
			item->retain();
			
			// Reorder to make the last used first
			cache.insertAtFront( read_item->fileName , item);
			
			// add to the list of pending tasks
			FileManagerCacheTask *task = new FileManagerCacheTask( read_item , item );
			tasks.push_back( task );
		}
		
		lock.unlock();
		
		lock.wakeUpStopLock( &stopLock );
		
		return answer;
	}
	
	/** 
	 Add a buffer to the cache
	 */
	
	void FileManagerCacheSystem::addWriteItem( FileManagerWriteItem *write_item )
	{
		// Add an element to the cache
		lock.lock();
		
		FileManagerCacheItem *item = cache.extractFromMap( write_item->fileName );
		
		if( item )
		{
			// Strange case... the best is to destroy incomming buffer and forget about it
			assert(false);
		}
		else
		{
			FileManagerCacheItem *new_item = new FileManagerCacheItem( write_item->fileName , write_item->buffer );
			cache.insertAtFront( write_item->fileName, new_item);
			size_in_cache += new_item->buffer->getSize();
			num_files_in_cache++;
			
			
		}
		
		lock.unlock();
	}
	
	
	
	/**
	 Notify that the write item is on disk
	 */
	
	void FileManagerCacheSystem::notifyWriteItemFinished( FileManagerWriteItem *write_item )
	{
		
		lock.lock();
		
		FileManagerCacheItem *item = cache.findInMap( write_item->fileName );
		assert( item );
		
		item->release();	// Release the counter added when created

		lock.unlock();
		
		// Free all possible memory
		//std::cout << "Free all memory in cache\n";
		//freeMemoryIfPossible(0.0);
		//std::cout << "Current cache: " << cache.size() << "\n";
		
		
	}
	
	
	std::string FileManagerCacheSystem::getStatus()
	{
		std::ostringstream output;
		output << num_files_in_cache << " elements in " << au::Format::string( size_in_cache ,"B" );
		return output.str();
	}

	
}
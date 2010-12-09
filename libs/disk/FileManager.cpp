

#include "FileManager.h" // Own interface


namespace ss
{

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
			while( (MemoryManager::shared()->getMemoryUsage() > 0.7) && freeMemoryIfPossible() );

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
	
	
	static FileManager* sharedFileManager=NULL;
	
	FileManager* FileManager::shared()
	{
		if( !  sharedFileManager )
			sharedFileManager = new FileManager();
		return sharedFileManager;
	}
	
	// Function to get the run-time status of this object
	void FileManager::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";
		getStatusFromMap( output , items, prefix_per_line );
		output << prefix_per_line << "Cache: " << cacheSystem.getStatus() << std::endl;
		output << prefix_per_line << "Statics: " << statistics.getStatus() << std::endl;
		
	}
	
	
	
}
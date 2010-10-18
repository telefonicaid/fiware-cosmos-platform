#include "DiskOperation.h"					// DiskOperation
#include "FileAccess.h"						// FileAccess
#include "DiskManagerDelegate.h"			// DiskManagerDelegate
#include "SamsonSetup.h"					// SamsonSetup
#include "DeviceDiskAccessManager.h"		// Own interface



namespace ss {
	
	void* runDeviceDiskAccessManagerThread(void * p)
	{
		((DeviceDiskAccessManager*)p)->runThread();
		return NULL;
	}

	DeviceDiskAccessManager::DeviceDiskAccessManager() : stopLock( &lock )
	{
		// Finish variable to controll threads
		finished = false;	

		// Get the max number of open files per devide from setup
		max_open_files = SamsonSetup::shared()->getInt( SETUP_max_open_files_per_device , 50 );
		
		// Create the thread for this disk operations
		pthread_create(&t, NULL, runDeviceDiskAccessManagerThread, this);
	}
	
	DiskOperation * DeviceDiskAccessManager::getNextOperation()
	{
		DiskOperation *o = NULL;
		
		lock.lock();
		if( operation.size() > 0)
		{
			o = operation.front();
			operation.pop_front();
		}
		lock.unlock();
		return o;
	}
	
	void DeviceDiskAccessManager::runThread()
	{
		while( !finished )
		{
			DiskOperation *o = getNextOperation();
			
			if( o)
				run (o);
			else
			{
				lock.lock();
				lock.unlock_waiting_in_stopLock( &stopLock );
			}
		}
	}
	
	void DeviceDiskAccessManager::addOperation(DiskOperation* o)
	{
		
		lock.lock();
		operation.push_back( o );	
		lock.unlock();
		
		// Wake up the thread if necessary
		lock.wakeUpStopLock( &stopLock );	
	}
	
	std::string DeviceDiskAccessManager::str()
	{
		std::ostringstream o;
		o << "Disk";
		o << " Files: " << files.size();
		o << " Ops: " << operation.size();
		o << " Read: " << readStatistics.str();
		o << " Write: " << writeStatistics.str();
		return o.str();
	}
		
	void DeviceDiskAccessManager::run( DiskOperation *o )
	{
		// Get the file information
		FileAccess *file = getFile( o->fileName  , o->mode );
		
		time_t start, stop;
		time(&start);
		
		bool result;
		if ( o->mode == "r" )
		{
			result = file->read(o->buffer, o->offset , o->size);
		}
		else
		{
			result = file->append(o->buffer, o->size);
		}
		
		time(&stop);
		
		if( result )
		{
			if ( o->mode == "r" )
				readStatistics.addSample( o->size , difftime(stop, start) );
			else
				writeStatistics.addSample( o->size , difftime(stop, start) );
		}
		
		// Nofity end of a task
		o->delegate->diskManagerNotifyFinish(o->idGet(), result);
		
		
	}
	
	FileAccess* DeviceDiskAccessManager::getFile( std::string fileName , std::string mode )
	{
		
		for ( std::list<FileAccess*>::iterator i = files.begin() ;  i != files.end() ; i++)
			if( (*i)->isFileName( fileName ) )
			{
				FileAccess *tmp = *i;
				
				// Put this file on top
				files.erase( i );
				files.push_front( tmp );
				
				return tmp;
			}
		
		// Remove if too much elements
		if( files.size() >= (size_t)max_open_files )	
		{
			FileAccess *tmp = files.back();
			files.pop_back();
			delete tmp;
		}
		
		// Create a new one
		FileAccess *f = new FileAccess( fileName , mode );
		files.push_front( f );
		return f;
	}
	
}

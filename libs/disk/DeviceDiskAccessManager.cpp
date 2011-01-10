#include "DiskOperation.h"					// DiskOperation
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
		// By default just one
		num_threads = SamsonSetup::shared()->num_io_threads_per_device;
		
		// Create the thread for this disk operations
		for (int i  =0 ; i < num_threads ; i++)
			pthread_create(&t[i], NULL, runDeviceDiskAccessManagerThread, this);
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
		while( true )
		{
			DiskOperation *o = getNextOperation();
			
			if( o )
			{
				run (o);
				delete o;
			}
			else
			{
				// Block until someone wake up you
				lock.lock();
				lock.unlock_waiting_in_stopLock( &stopLock );
			}
		}
	}
	
	void DeviceDiskAccessManager::addOperation(DiskOperation* o)
	{
		
		lock.lock();
		
		if( o->mode == "w" )
			operation.push_front( o );	
		else
			operation.push_back( o );	
		
		lock.unlock();
		
		// Wake up the thread if necessary
		lock.wakeUpStopLock( &stopLock );	
	}
	
		
	void DeviceDiskAccessManager::run( DiskOperation *o )
	{
		
		bool result;
		struct timeval start,stop;
		gettimeofday(&start, NULL);
		
		if( o->mode == "w" )
		{
			// Create a new file

			FILE *file = fopen( o->fileName.c_str() , "w" );
			assert( file );
			
			if( fwrite(o->buffer->getData(), o->size, 1 , file) == 1 )
			{
				fflush(file);
				result = true;
			}
			else
				result = false;
						
			gettimeofday(&stop, NULL);
			if( result )
				statistics.add(DiskStatistics::write, o->size, DiskStatistics::timevaldiff( &start , &stop) );

			fclose(file);
		}
		
		if( o->mode == "r")
		{
			// Create a new one
			//FileAccess *file = new FileAccess( o->fileName , o->mode );
			//result = file->read(o->read_buffer, o->offset , o->size);
			
			FILE *file = fopen( o->fileName.c_str() , "r" );
			assert(file);

			fseek(file, o->offset, SEEK_SET);				// Seek to the rigth offset
			
			result = ( fread(o->read_buffer, o->size, 1, file) == 1 );
			
			gettimeofday(&stop, NULL);
			if( result )
				statistics.add(DiskStatistics::read, o->size, DiskStatistics::timevaldiff( &start , &stop) );

			fclose(file);
		}
		
		// Nofity end of a task
		o->delegate->diskManagerNotifyFinish(o->idGet(), result);
		
	}
	
	
	std::string DeviceDiskAccessManager::getStatus(  )
	{
		std::ostringstream output;
		lock.lock();
		output << operation.size() << " operations " << statistics.getStatus();
		lock.unlock();
		return output.str();
	}

	
		
	
}

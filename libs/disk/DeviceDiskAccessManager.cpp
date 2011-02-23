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

		// Multithread has been canceled
		// We have removed the FileManager cache so read operations over save is assured sice read operations arrives latter that write operations
		// If multithread is reactivated then, we have to make sure read operations wait until write has finished
		num_threads = 1;	

		// Set the number of running operations to 0
		running_operations = 0;
		
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
				lock.lock();
				running_operations++;
				lock.unlock();
				
				
				run (o);
				delete o;
				
				lock.lock();
				running_operations--;
				lock.unlock();
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
			operation.push_back( o );	
		else
			operation.push_back( o );	
		
		lock.unlock();
		
		// Wake up the thread if necessary
		lock.wakeUpStopLock( &stopLock );	
	}
	
		
	void DeviceDiskAccessManager::run( DiskOperation *o )
	{
		
		struct timeval start,stop;
		gettimeofday(&start, NULL);
		
		if( o->mode == "w" )
		{
			// Create a new file
			
			std::ostringstream fn;
			fn << SamsonSetup::shared()->dataDirectory << "/" << o->fileName;
			
			FILE *file = fopen( fn.str().c_str() , "w" );
			if ( !file )
				o->setError("Error opening file");
			else
			{
				
				if( fwrite(o->buffer->getData(), o->size, 1 , file) == 1 )
				{
					fflush(file);
					
					gettimeofday(&stop, NULL);
					statistics.add(DiskStatistics::write, o->size, DiskStatistics::timevaldiff( &start , &stop) );
				}
				else
				{
					o->setError("Error writing data to the file");
				}
			}
						
			fclose(file);
		}
		
		if( o->mode == "r")
		{
			// Create a new one
			//FileAccess *file = new FileAccess( o->fileName , o->mode );
			//result = file->read(o->read_buffer, o->offset , o->size);
			
			std::ostringstream fn;
			fn << SamsonSetup::shared()->dataDirectory << "/" << o->fileName;
			
			FILE *file = fopen(fn.str().c_str() , "r" );
			if( !file )
				o->setError("Error opening file");
			else
			{
				if( fseek(file, o->offset, SEEK_SET) != 0)
					o->setError("Error in fseek operation");
				else
				{
					if ( fread(o->read_buffer, o->size, 1, file) == 1 )
					{
						gettimeofday(&stop, NULL);
						statistics.add(DiskStatistics::read, o->size, DiskStatistics::timevaldiff( &start , &stop) );
					}
					else
					{
						if( o->size != 0 )	// If size is 0, fread returns 0 and it is correct
							o->setError("Error while reading data from file");
					}
				}

				fclose(file);
			}
		}
		
		// Nofity end of a task
		o->delegate->diskManagerNotifyFinish(o->idGet(), o->error , o->error_message );
		
	}
	
	
	std::string DeviceDiskAccessManager::getStatus(  )
	{
		std::ostringstream output;
		lock.lock();
		output << "Run:" << running_operations << " Wait:" << operation.size() << " Statis:" << statistics.getStatus();
		lock.unlock();
		return output.str();
	}

	
		
	
}

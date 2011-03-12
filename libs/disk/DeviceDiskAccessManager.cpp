#include "DiskOperation.h"					// DiskOperation
#include "DiskManagerDelegate.h"			// DiskManagerDelegate
#include "SamsonSetup.h"					// SamsonSetup
#include "DeviceDiskAccessManager.h"		// Own interface
#include "traceLevels.h"					// LmtDisk
#include "logMsg.h"							// LM_T

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
		
		// Create the thread for this disk operations
		for (int i  =0 ; i < num_threads ; i++)
			pthread_create(&t[i], NULL, runDeviceDiskAccessManagerThread, this);
	}
	
	DiskOperation * DeviceDiskAccessManager::getNextOperation()
	{
		DiskOperation *o = NULL;
		
		lock.lock();
		
		if( pending_operations.size() > 0)
		{
			o = pending_operations.extractFront();
			running_operations.insert( o );
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
				
				lock.lock();
				running_operations.erase( o );
				lock.unlock();
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
		pending_operations.push_back( o );	
		lock.unlock();
		
		// Wake up the thread if necessary
		lock.wakeUpStopLock( &stopLock );	
	}
	
		
	void DeviceDiskAccessManager::run( DiskOperation *o )
	{
		
		LM_T( LmtDisk , ("DiskManager: Running operation %s", o->getDescription().c_str() ));
		
		struct timeval start,stop;
		gettimeofday(&start, NULL);
		
		if( o->type == DiskOperation::write )
		{
			// Create a new file
			
			std::ostringstream fn;
			fn << SamsonSetup::shared()->dataDirectory << "/" << o->fileName;

			LM_T( LmtDisk , ("DiskManager: Opening file %s to write", fn.str().c_str() ));
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
		
		if( o->type == DiskOperation::read )
		{
			// Create a new one
			//FileAccess *file = new FileAccess( o->fileName , o->mode );
			//result = file->read(o->read_buffer, o->offset , o->size);
			
			std::ostringstream fn;
			fn << SamsonSetup::shared()->dataDirectory << "/" << o->fileName;

			LM_T( LmtDisk , ("DiskManager: Opening file %s to read", fn.str().c_str() ));
			
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
		
		if( o->type == DiskOperation::remove)
		{
			std::ostringstream fn;
			fn << SamsonSetup::shared()->dataDirectory << "/" << o->fileName;
			
			LM_T( LmtDisk , ("DiskManager: Removing file %s to read", fn.str().c_str() ));
			
			// Remove the file
			int c = remove( fn.str().c_str() );
			if( c != 0 )
				o->setError("Error while removing file");
		}

		// Nofity end of a task
		o->delegate->diskManagerNotifyFinish( o->idGet(), o->error.isActivated() , o->error.getMessage() );
		
	}
	
	
	std::string DeviceDiskAccessManager::getStatus(  )
	{
		std::ostringstream output;
		lock.lock();

		output << "\n\t\tRunning:  ";
		for ( std::set<DiskOperation*>::iterator i = running_operations.begin() ; i != running_operations.end() ; i++)
			output << "\n\t\t\t" << (*i)->getDescription();

		output << "\n\t\tPending:  ";
		for ( au::list<DiskOperation>::iterator i = pending_operations.begin() ; i != pending_operations.end() ; i++)
			output << (*i)->getShortDescription() << ",";
		
		output << "\n\t\tStatis:   " << statistics.getStatus();
		lock.unlock();
		return output.str();
	}

	
		
	
}

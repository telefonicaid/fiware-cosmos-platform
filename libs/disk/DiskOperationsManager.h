

#ifndef _H_DISK_OPERATIONS_MANAGER
#define _H_DISK_OPERATIONS_MANAGER

#include <stdio.h>			// fopen , fclose, ...
#include <string>			// std::string
#include <map>				// std::map
#include <set>				// std::set
#include <list>				// std::list
#include "Lock.h"			// au::Lock
#include "StopLock.h"		// au::StopLock
#include <pthread.h>        /* pthread_mutex_init, ...                  */
#include <sys/stat.h>		// stat(.)
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <Format.h>			// au::Format
#include <time.h>			// clock(.)

#include "DiskManagerDelegate.h"		// DiskManagerDelegate
namespace ss {
	
	
	class DiskOperationsManager : public ss::DiskManagerDelegate
	{
		ss::DiskManager *dm;			// Pointer to the shared DiskManager object
		std::set<size_t> codes;			// Codes of all pending operations
		std::set<size_t> failedCodes;	// Codes of all pending operations
		
		au::Lock lock;					// Lock to protect multiple notifications from multiple threads
		
	public:
		
		DiskOperationsManager()
		{	
			// Get the pointer to the shared Disk Manager
			dm = ss::DiskManager::shared();
		}
		
		virtual void diskManagerNotifyFinish( size_t id , bool success )
		{
			lock.lock();
			
			if( !success )
				failedCodes.insert( id );
			codes.erase( id );
			
			lock.unlock();
			
			// Show information about disk access
			dm->showStatus();
		}
		
		void read( char *buffer , size_t size ,  std::string fileName , size_t offset )
		{
			lock.lock();
			
			size_t id = dm->read( buffer , size , fileName , offset , this );
			if( id )
				codes.insert( id );
			
			lock.unlock();
		}
		
		void write( char *buffer , size_t size ,  std::string fileName  )
		{
			lock.lock();
			
			size_t id = dm->write( buffer , size , fileName , this );
			if( id )
				codes.insert( id );
			
			lock.unlock();
			
		}
		
		bool finishedAllOperation()
		{
			lock.lock();
			bool f = ( codes.size() == 0 );
			lock.unlock();
			return f;
		}
		
		void waitUntilComplete()
		{
			while (true) {
				if( finishedAllOperation() )
					return;
				sleep(1);
			}
			
		}
		
	};		
}

#endif
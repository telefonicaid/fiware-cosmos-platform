#ifndef _H_DISK_MANAGER
#define _H_DISK_MANAGER

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
#include "Buffer.h"			// ss::Buffer

namespace ss {
	
	class DeviceDiskAccessManager;
	class DiskOperation;
	class DiskManagerDelegate;
	
	class DiskManager
	{
		// Global lock for thread safe implementation
		au::Lock lock;
		
		// All the files that are open
		std::map <dev_t , DeviceDiskAccessManager*> item;		// All partitions we can access
		size_t counter_id;
		
		DiskManager();
		
	public:

		/**
		 Singleton implementation of disk manager
		 */
		static DiskManager* shared();
		
		/**
		 Schedule a read operation
		 Return inmediatelly
		 Latter a notification is send to the delegate
		 */
		size_t read( char *data , std::string fileName , size_t offset , size_t size , DiskManagerDelegate *delegate );

		/**
		 Schedule a write operation
		 Return inmediatelly
		 Latter a notification is send to the delegate
		 */
		
		size_t write( Buffer* buffer ,  std::string fileName , DiskManagerDelegate *delegate );
		
		/**
		 Show status for debugging
		 */
		
		void showStatus();
		
	private:
		
		
		/** 
		 Get the device manager for this device
		 */
		
		DeviceDiskAccessManager *getDeviceDiskAccessManagerForDev( dev_t st_dev );

		/** 
		 Add operation to the rigth device manager
		 */
		
		bool addOperation( DiskOperation *o );
		
	};

}

#endif

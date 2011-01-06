#ifndef _H_DEVICE_DISK_ACCESS_MANAGER
#define _H_DEVICE_DISK_ACCESS_MANAGER

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
#include "DeviceDiskAccessStatistics.h"	//DeviceDiskAccessStatistics
#include "au_map.h"			// au::map
#include "Status.h"				// au::Status
#include "DiskStatistics.h"		// ss::DiskStatistics
#include "Status.h"				// au::Status

#define MAX_NUM_THREADS_PER_DEVICE 10

namespace ss {
	

	class DiskOperation;
	class FileAccess;
	
	void* runDeviceDiskAccessManagerThread(void * p);
	
	class DeviceDiskAccessManager 
	{
		au::Lock lock;		
		au::StopLock stopLock;
		
		std::list<DiskOperation*> operation;			// List of pending operations

		int num_threads;							// Number of threads working over this device		
		pthread_t t[MAX_NUM_THREADS_PER_DEVICE];	// Thread to read/write in this device
		
	public:

		// Statistical information
		DiskStatistics statistics;
		
		DeviceDiskAccessManager();
		
		DiskOperation * getNextOperation();
		
		void runThread();
		
		void addOperation(DiskOperation* o);
		
		// Function to get the run-time status of this object
		std::string getStatus( );
		
	private:
		
		void run( DiskOperation *o );
		
	};
}

#endif

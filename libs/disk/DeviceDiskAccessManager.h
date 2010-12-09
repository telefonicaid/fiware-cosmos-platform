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

namespace ss {
	

	class DiskOperation;
	class FileAccess;
	
	void* runDeviceDiskAccessManagerThread(void * p);
	
	class DeviceDiskAccessManager : public au::Status
	{
		au::Lock lock;
		au::StopLock stopLock;
		std::list<DiskOperation*> operation;			// List of pending operations
		
		au::map<std::string , FileAccess> files_read;	// List of open files to read
		
		pthread_t t;									// Thread to read/write in this device
		bool finished;									// Flag to finish the background thread
		
		int max_open_files;
		
		// Statistical information
		DiskStatistics statistics;
		

	public:
		
		DeviceDiskAccessManager();
		
		DiskOperation * getNextOperation();
		
		void runThread();
		
		void addOperation(DiskOperation* o);
		
		// Function to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );
		
	private:
		
		void run( DiskOperation *o);
		
		FileAccess* getFile( std::string fileName , std::string mode );


		
	};
}

#endif

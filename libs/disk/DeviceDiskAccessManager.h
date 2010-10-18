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

namespace ss {
	

	class DiskOperation;
	class FileAccess;
	
	void* runDeviceDiskAccessManagerThread(void * p);
	
	class DeviceDiskAccessManager
	{
		au::Lock lock;
		au::StopLock stopLock;
		std::list<DiskOperation*> operation;			// List of pending operations
		
		std::list<FileAccess*> files;					// Information about open files
		
		pthread_t t;									// Thread to read/write in this device
		bool finished;									// Flag to finish the background thread
		
		
		int max_open_files;
		
		// Statistical information
		DeviceDiskAccessStatistics writeStatistics;
		DeviceDiskAccessStatistics readStatistics;
		
	public:
		
		DeviceDiskAccessManager();
		
		DiskOperation * getNextOperation();
		
		void runThread();
		
		void addOperation(DiskOperation* o);
		
		std::string str();
		
	private:
		
		void run( DiskOperation *o);
		
		FileAccess* getFile( std::string fileName , std::string mode );

		
		
	};
}

#endif

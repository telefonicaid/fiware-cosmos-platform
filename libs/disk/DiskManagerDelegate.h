#ifndef _H_DISK_MANAGER_DELEGATE
#define _H_DISK_MANAGER_DELEGATE

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


namespace ss {
	
	class FileManagerReadItem;
	class FileManagerWriteItem;
	
	/**
	 Interface to receive notifictions from DiskManager
	 */
	
	class DiskManagerDelegate
	{
	public:
		// Notify that this operation has finished
		virtual void diskManagerNotifyFinish(size_t id, bool success) = 0;	
	};
	
	/**
	 General item of the file manager
	 */
	
	class FileManagerDelegate
	{
	public:
		virtual void notifyFinishReadItem( FileManagerReadItem *item  ) = 0;	
		virtual void notifyFinishWriteItem( FileManagerWriteItem *item  ) = 0;	
	};
	
	
}

#endif

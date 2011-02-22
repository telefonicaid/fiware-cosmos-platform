#ifndef _H_DISK_OPERATION
#define _H_DISK_OPERATION

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
#include "Buffer.h"			// Buffer used to read or write

namespace ss {
	
	class DiskManagerDelegate;
	
	class DiskOperation
	{
		size_t _id;
		
	public:
		
		std::string fileName;			// FileName to open
		std::string mode;				// Mode to open
		
		Buffer * buffer;				// Buffer used when writing
		char *read_buffer;				// Buffer used when reading from disk	
		
		size_t size;					// Size to read/write
		size_t offset;					// Offset inside the file ( onlu for read operations )
		dev_t st_dev;					// Device where this file is stored

		DiskManagerDelegate *delegate;	// Who should be notified
		
		bool error;						// Flag to indicate that there was an error
		std::string error_message;		// Error message
		
		DiskOperation( size_t id );
		
		size_t idGet();
		
		static std::string directoryPath( std::string path );
		
		bool setDevice( );
		
		void setError( std::string message );
	};
	
}

#endif

/* ****************************************************************************
 *
 * FILE                     DiskManagerOperation.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

/*
 
 Note:
 
 DiskManagerOperation is the class that contains all the information for a particular operation
 
 It should contain the name of the file, the size, the offset, etc. ( basically all the information needed for the operation )
 It shouls also contain the buffer where data is writed or readed from
 It finally contains a pointer to the delegate to notify when finished

 */


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
#include "Error.h"			// ss::Error

namespace ss {
	
	class DiskManagerDelegate;
	
	class DiskOperation
	{
		size_t _id;
		
	public:

		typedef enum
		{
			read,
			write,
			remove
		} DiskOperationType;		
		
		std::string fileName;				// FileName to open

		DiskOperationType type;	// Type of operation ( read, write , remove , etc.. )
		
		Buffer * buffer;					// Buffer used when writing
		char *read_buffer;					// Buffer used when reading from disk	
			
		size_t size;						// Size to read/write
		size_t offset;						// Offset inside the file ( only for read operations )
		
		dev_t st_dev;						// Device where this file is stored

		DiskManagerDelegate *delegate;		// Delegate to notify when finish
	
		au::Error error;					// Management of the error during this operation
		
		DiskOperation( size_t id );

		size_t idGet();
		static std::string directoryPath( std::string path );
		void setError( std::string message );
		bool setDevice( );

		std::string getDescription();
		std::string getShortDescription();
		
	};
	
}

#endif

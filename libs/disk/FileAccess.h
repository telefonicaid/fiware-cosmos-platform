#ifndef _H_FILE_ACCESS
#define _H_FILE_ACCESS

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
	
	class FileAccess
	{
		
	public:
		FILE *_file;				// File pointer if it is still open
		std::string _fileName;		// A copy of the fileName
		std::string _mode;			// Mode of the fopen
		
		void init( )
		{
			if( _file )
				fclose(_file);			
			_file = fopen(_fileName.c_str(), _mode.c_str());
		}
		
		void close()
		{
			if( _file) 
			{
				fclose(_file);
				_file = NULL;
			}
		}
		
	public:
		
		FileAccess( std::string fileName , std::string mode )
		{
			_fileName = fileName;
			_mode = mode;
			
			_file = NULL;
			init();
		}
		
		~FileAccess()
		{
			close();
		}
		
		
		
		void setMode( std::string mode )
		{
			_mode = mode;
			close();
			init();
		}
		
		bool read( char *buffer , size_t offset , size_t size)
		{
			assert( _mode == "r" );
			assert( _file );
			
			// Seek to the rigth offset
			fseek(_file, offset, SEEK_SET);
			
			if( fread(buffer, size, 1, _file) == 1 )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		
		bool append( char *buffer , size_t size )
		{
			
			assert( _mode == "w" );
			assert( _file );
			
			if( fwrite(buffer, size,1 , _file) == 1 )
			{
				fflush(_file);
				return true;
			}
			else
				return false;
		}
		
		
		bool isFileName( std::string& fileName )
		{
			return ( _fileName == fileName );
			
		}
		
		
	};

}

#endif

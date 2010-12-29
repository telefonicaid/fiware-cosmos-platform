#ifndef _H_DELILAH_LOAD_DATA_PROCESS
#define _H_DELILAH_LOAD_DATA_PROCESS

#include "Lock.h"				// au::Lock
#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include "samson.pb.h"		// ss::network
#include "DelilahComponent.h"			// ss::DelilahComponent
#include "Delilah.h"			// ss::Delilah

#include <sstream>			// std::ostringstream
#include "Format.h"			// au::Format

namespace ss {

	void* runThreadDelilahLoadDataProcess(void *p);

	
	class Delilah;
	class Buffer;
	
	
	class TXTFileSet
	{
		
		std::vector<std::string> fileNames;		// List of files to read
		std::ifstream inputStream;				// Stream of the file we are reading	
		
		int file;
		bool finish;
		
		char previousBuffer[ 10000 ];			// Part of the previous buffer waiting for the next read
		size_t previousBufferSize;

		std::vector<std::string> failedFiles;		// List of files that could not be uploaded		
		
	public:
		
		TXTFileSet( std::vector<std::string> &_fileNames )
		{
			fileNames = _fileNames;
			
			previousBufferSize = 0;
			
			finish = false;
			file = 0;	
			
			openNextFile();
		}
		
		void openNextFile()
		{
			while ( !inputStream.is_open() ) 
			{
				
				if( file >= (int)fileNames.size() )
				{
					finish = true;
					return;
				}
				
				inputStream.open( fileNames[file].c_str() );
				if( !inputStream.is_open() )
					failedFiles.push_back( fileNames[file] );	// Add to the list of failed files
				file++;
				
			}
		}
		
		
		bool isFinish()
		{
			return finish;
		}
		
		// Read as much as possible breaking in lines
		void fill( Buffer *b );

		std::vector<std::string> getFailedFiles()
		{
			return failedFiles;
		}
		
		
	};
	
	
	// All the information related with a load process
	class DelilahUploadDataProcess : public DelilahComponent
	{
		au::Lock lock;			// Lock mechanish to protext ( async confirmation of the workers )
		
		int num_workers;		// Total number of workers
		int worker;				// Current worker
		
		pthread_t t;			// Thread of this process
		
		TXTFileSet fileSet;		// Input txt files
		
		// Set of ids pending to be confirmed
		std::set<size_t> pending_ids;
		size_t id_counter;

		bool finish;			// Flag to notify that all data has been sent to workers
		bool completed;			// Flag to notify that the process is complete ( everything is confirmed )
		

		std::string queue;
		
		// Sumary information
		size_t uploadedSize;	// Total size of uploaded files
		size_t totalSize;		// Total size to be uploaded ( all files )
		
		bool error;
		std::string error_message;
		
		
		// Created files
		std::vector<network::File> created_files;
		
		struct timeval init_time;
		
	public:

		DelilahUploadDataProcess( std::vector<std::string> &fileNames , std::string _queue );		
		
		void run();		
		void _run();	// Method only called by a separeted thread		
		
		void fillLoadDataConfirmationMessage( network::UploadDataConfirmation *confirmation );		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		
		std::string getStatus();
		
		
	};	
	
	
}

#endif

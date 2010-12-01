
#ifndef _H_DELILAH_DOWNLOAD_DATA_PROCESS
#define _H_DELILAH_DOWNLOAD_DATA_PROCESS


#include "DelilahComponent.h"			// ss::DelilahComponent
#include <string>						// std::string
#include "Delilah.h"					// ss::Delilah
#include "DelilahClient.h"				// ss::DelilahClient
#include <list>							// std::list

namespace ss {

	class DelilahDownloadDataProcess : public DelilahComponent
	{
		std::string queue;
		std::string fileName;
		
		bool error;
		std::string error_message;
		
		bool queue_info;	// When requesting for that particular info
		
		int num_files_to_download;
		int num_files_downloaded;
		
		au::Lock lock;				// Local lock for the thread that write data to disk
		au::StopLock stopLock;		// Waiting room for the thread
		FILE *file;					// File descriptor
		
		std::list<Buffer*> buffers;	// List of buffers pendign to be writen
		
		pthread_t t;				// Secondary thread to write stuff to disk
		
	public:
		
		DelilahDownloadDataProcess( std::string _queue , std::string _fileName );

		
		void run();
		
		
		void runThread();
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		
		virtual std::string getStatus()
		{
			std::ostringstream output;
			output << "["<< id << "]Downloading queue " << queue << " to " << fileName;
			output << ": Total files: " << num_files_downloaded << "/" << num_files_to_download;
			output << " Buffers: " << buffers.size() << "\n";
			return output.str();
		}

		
	};

}

#endif
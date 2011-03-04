
#ifndef _H_DELILAH_DOWNLOAD_DATA_PROCESS
#define _H_DELILAH_DOWNLOAD_DATA_PROCESS


#include "DelilahComponent.h"			// ss::DelilahComponent
#include <string>						// std::string
#include "Delilah.h"					// ss::Delilah
#include "DelilahClient.h"				// ss::DelilahClient
#include <list>							// std::list
#include "Error.h"						// au::Error

namespace ss {

	class DelilahDownloadDataProcess : public DelilahComponent
	{
		
		typedef enum
		{
			uninitialized,
			waiting_controller_init_response,				// Waiting for the init response
			downloading_files_from_workers,					// Send requests to get files from workers
			waiting_file_downloads_confirmations,			// Files are completelly scheduled, so we are waiting confirmations form workers
			waiting_controller_finish_response,				// Pending to receive the final message from controller
			finish,
			finish_with_error
		} DownloadStatus;
		
		DownloadStatus status;		// Status of this operations
		
		std::string queue;
		std::string fileName;

		
		bool queue_info;	// When requesting for that particular info
		
		int num_files_to_download;
		int num_files_downloaded;
		
		au::Lock lock;				// Local lock for the thread that write data to disk
		au::StopLock stopLock;		// Waiting room for the thread
		FILE *file;					// File descriptor
		
		std::list<Buffer*> buffers;	// List of buffers pendign to be writen
		
		pthread_t t;				// Secondary thread to write stuff to disk
		
		bool show_on_screen;		// Flag to indicate if we should show this at the end on screen
		
		size_t total_size;			// total accumulated size
		
		
		// Copy of the message received from the controller for this operation
		
		ss::network::DownloadDataInitResponse *download_data_init_response;
		
	public:
		
		au::Error error;	// Error management
		
		DelilahDownloadDataProcess( std::string _queue , std::string _fileName , bool show_on_screen );

		void run();
		
		void runThread();
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		
		virtual std::string getStatus();

		std::string getDescription(); 
		
		
	};

}

#endif
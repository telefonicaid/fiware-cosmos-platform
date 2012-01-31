/*
 
#ifndef _H_DELILAH_LOAD_DATA_PROCESS
#define _H_DELILAH_LOAD_DATA_PROCESS

#include "au/Lock.h"				// au::Lock
#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include "samson/common/samson.pb.h"		// samson::network
#include "DelilahComponent.h"			// samson::DelilahComponent
#include "samson/delilah/Delilah.h"			// samson::Delilah

#include <sstream>			// std::ostringstream
#include "au/string.h"			// au::Format
#include <list>				// std::list
#include "TXTFileSet.h"		// samson::TXTFileSet
#include "au/ErrorManager.h"			// au::ErrorManager

namespace samson {

	void* runThreadDelilahLoadDataProcess(void *p);

	
	class Delilah;
	class Buffer;
	
	
	// All the information related with a load process
	class DelilahUploadComponent : public DelilahComponent
	{
		
		typedef enum
		{
			uninitialized,
			waiting_controller_init_response,				// Waiting for the init response
			sending_files_to_workers,						// Sending files and receiving response messaged
			waiting_file_upload_confirmations,				// Files are completely scheduled, so we are waiting confirmations form controllers
			waiting_controller_finish_response,				// Pending to receive the final message from controller
			finish_with_error,								// Some error occurred in the process
			finish											// Everything finished fine
		} UploadStatus;

		UploadStatus status;			// Status of the upload process
		
		au::Lock lock;					// Lock mechanism to protect ( async confirmation of the workers )
		
		int num_workers;				// Total number of workers
		
		pthread_t t;					// Thread of this process

		// Data information
		
		std::string queue;				// Name of the queue we are uploading
		TXTFileSet fileSet;				// Input txt files
		
		size_t num_files;				// Num files generated
		size_t num_confirmed_files;		// Num files confirmed by workers ( uploaded )
		
		int num_threads;				// Number of parallel threads to wait if necessary
		int max_num_threads;			// Maximum number of parallel threads
		
		// Message prepared for the final confirmation
		samson::network::UploadDataFinish *upload_data_finish;						
		
		// Id of the operation at the controller
		size_t load_id;
		
		// Summary information
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and scheduled to the network )
		
		size_t uploadedSize;			// Total size of uploaded files
		size_t uploadedCompressedSize;	// Total size uploaded to workers ( compressed )

        
	public:

		
        // Public since it has to be accessible from the thread
		bool compression;	
		
		DelilahUploadComponent( std::vector<std::string> &fileNames , std::string _queue , bool _compression , int _max_num_threads );		
		~DelilahUploadComponent();
		
		void run();     // Main method to start the upload process ( sending an init message to controller )
		
		void _run();	// Method only called by a separated thread
		
        // Functions to create the final confirmation message
		void fillUpLoadDataFinishMessage( network::UploadDataFinish *confirmation );		

        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
		
		std::string getStatus();		
        std::string getShortStatus();
		
		void _runCompressThread();	// Method executed by all the compression threads
		
		void finishCompressionThread( size_t process_size );		
		
		std::string showProgress( std::string title,  size_t size );

	};	
	
	
}

#endif
 
 */

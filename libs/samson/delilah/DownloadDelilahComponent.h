
#ifndef _H_DELILAH_DOWNLOAD_DATA_PROCESS
#define _H_DELILAH_DOWNLOAD_DATA_PROCESS


#include <string>						// std::string
#include <list>							// std::list

#include "au/ErrorManager.h"			// au::ErrorManager
#include "au/CounterCollection.h"             // au::CounterCollection

#include "engine/Object.h"              // engine::Object

#include "samson/delilah/Delilah.h"		// samson::Delilah

#include "DelilahComponent.h"			// samson::DelilahComponent
#include "DelilahClient.h"				// samson::DelilahClient

namespace samson {

    
	class DelilahDownloadComponent : public DelilahComponent , public engine::Object
	{
		
        bool force_flag;            // Flag to force delete of directory first
		std::string queue;          // Queue we are downloading
		std::string fileName;       // Local filename to save ( this is a directory )

		bool received_init_response;    // Flag to mark when we receive init response
		int num_files_to_download;      // Total number of files to be downloaded ( included in the first response message )
		int num_files_downloaded;       // Number of files already downloaded
        int num_write_operations;       // Number of pending write operations
				
        au::CounterCollection<int> counter_per_worker;     // Count the number of outputs per worker
        
	public:
		
		DelilahDownloadComponent( std::string _queue , std::string _fileName , bool force_flag );

		void run();
		
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

        void notify( engine::Notification* notification );

        void check();
		
		std::string getStatus();
        std::string getShortStatus();
		std::string getDescription(); 
		
		
	};

}

#endif
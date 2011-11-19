#ifndef _H_PUSH_DELILAH_COMPONENT
#define _H_PUSH_DELILAH_COMPONENT

#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <list>				// std::list

#include "au/string.h"			// au::Format
#include "au/ErrorManager.h"			// au::ErrorManager
#include "au/Cronometer.h"      // au::Cronometer
#include "au/Lock.h"				// au::Lock

#include "engine/Engine.h"          // engine::Object
#include "engine/Object.h"          // engine::Object
#include "engine/DiskManager.h"     // engine::DiskManager

#include "samson/common/samson.pb.h"		// samson::network

#include "samson/delilah/Delilah.h"			// samson::Delilah
#include "DelilahComponent.h"			// samson::DelilahComponent

#include "TXTFileSet.h"		// samson::TXTFileSet


namespace samson {

	
	class Delilah;
	class Buffer;
	
	
	// All the information related with a load process
	class PushDelilahComponent : public DelilahComponent , engine::Object
	{
		
        std::set<std::string> queues;				// Name of the stream-queue we are uploading
        
		// Sumary information
        
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and squeduled to the network )
		size_t uploadedSize;			// Total size of uploaded files

        bool finish_process;            // Flag to indicate that we have process all input data
        
        au::Cronometer cronometer;
		
        DataSource *dataSource;
        
        // Input txt files
        //TXTFileSet fileSet;				
        
	public:

		// Error log ( public since it is access from delilah )
		au::ErrorManager error;
		
		
		PushDelilahComponent( DataSource * _dataSource , std::string _queue  );		
        void addQueue( std::string  _queue );
		~PushDelilahComponent();
		
        // Function to start running
        void run();
        
        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// Function to get the status
		std::string getStatus();		
        
        // Notifications
        void notify( engine::Notification* notification );
        
        // Virtual in DelilahComponent
        std::string getShortDescription();
        
    private:
        
        void requestMemoryBuffer();

	};	
	
	
}

#endif

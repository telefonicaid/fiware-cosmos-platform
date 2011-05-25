#ifndef _H_DELILAH_PUSH_COMPONENT
#define _H_DELILAH_PUSH_COMPONENT

#include "au/Lock.h"				// au::Lock
#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include "samson/common/samson.pb.h"		// samson::network
#include "DelilahComponent.h"			// samson::DelilahComponent
#include "samson/delilah/Delilah.h"			// samson::Delilah

#include <sstream>			// std::ostringstream
#include "au/Format.h"			// au::Format
#include <list>				// std::list
#include "TXTFileSet.h"		// samson::TXTFileSet
#include "au/Error.h"			// au::Error
#include "au/Cronometer.h"      // au::Cronometer

namespace samson {

	
	class Delilah;
	class Buffer;
	
	
	// All the information related with a load process
	class PushComponent : public DelilahComponent , engine::NotificationListener
	{
		
		int num_workers;				// Total number of workers

		// Data information

		std::string queue;				// Name of the stream-queue we are uploading
        
		// Sumary information
        
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and squeduled to the network )
		size_t uploadedSize;			// Total size of uploaded files

        au::Cronometer cronometer;
        
		// Worker to send the next packet
		int worker;
		
        // Input txt files
        TXTFileSet fileSet;				
        
	public:

		// Error log ( public since it is access from delilah )
		au::Error error;
		
		
		PushComponent( std::vector<std::string> &fileNames , std::string _queue  );		
		~PushComponent();
		
        // Function to start running
        void run();
        
        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// Function to get the status
		std::string getStatus();		
		
        
        // Notifications
        
        void notify( engine::Notification* notification );
        bool acceptNotification( engine::Notification* notification );

        
    private:
        
        void requestMemoryBuffer();

	};	
	
	
}

#endif

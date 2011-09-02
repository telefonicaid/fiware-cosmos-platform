#ifndef _H_DELILAH_PUSH_COMPONENT
#define _H_DELILAH_PUSH_COMPONENT

#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <list>				// std::list

#include "au/Format.h"			// au::Format
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
	class PushComponent : public DelilahComponent , engine::Object
	{
		
		int num_workers;				// Total number of workers

		// Data information

        std::set<std::string> queues;				// Name of the stream-queue we are uploading
        
		// Sumary information
        
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and squeduled to the network )
		size_t uploadedSize;			// Total size of uploaded files

        bool finish_process;            // Flag to indicate that we have process all input data
        
        au::Cronometer cronometer;
        
		// Worker to send the next packet
		int worker;
		
        DataSource *dataSource;
        
        // Input txt files
        //TXTFileSet fileSet;				
        
	public:

		// Error log ( public since it is access from delilah )
		au::ErrorManager error;
		
		
		PushComponent( DataSource * _dataSource , std::string _queue  );		
        void addQueue( std::string  _queue );
		~PushComponent();
		
        // Function to start running
        void run();
        
        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// Function to get the status
		std::string getStatus();		
        std::string getShortStatus();
		
        
        // Notifications
        void notify( engine::Notification* notification );

        
    private:
        
        void requestMemoryBuffer();

	};	
    
    
    class PopComponent : public DelilahComponent , public engine::Object
    {
    public:
        
        std::string queue;                  // Name of the queue we are recovering
        std::string fileName;               // Name of the file to create
        
        int num_workers;                    // Number of workers ( where we have send the request )
        int num_finish_worker;              // Number of workers that have confirmed that everything has been sent
        
        int num_write_operations;           // Number of pending local write operations
        
        int num_outputs;                    // counter of the number of outputs ( to change the filename )
        
        bool force_flag;                    // Flag to remove previos directory
        
        PopComponent( std::string _queue , std::string _fileName , bool force_flag );
        
        ~PopComponent();
        
        void run();
        
        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
 
        std::string getShortStatus();
        
		// Function to get the status
		std::string getStatus();

        // Function to send a notification to this object
        void notify( engine::Notification* notification );
    
        void check();
        
    };
    
	
	
}

#endif

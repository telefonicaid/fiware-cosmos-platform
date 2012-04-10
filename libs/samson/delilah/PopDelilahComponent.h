#ifndef _H_POP_DELILAH_COMPONENT
#define _H_POP_DELILAH_COMPONENT

#include <fstream>				// ifstream
#include <vector>				// std::vector
#include <set>				// std::vector
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <list>				// std::list

#include "au/string.h"			// au::Format
#include "au/ErrorManager.h"			// au::ErrorManager
#include "au/Cronometer.h"      // au::Cronometer
#include "au/Descriptors.h"         // au::CounterCollection
#include "au/CounterCollection.h"   // au::CounterCollection

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
	

    class PopDelilahComponent : public DelilahComponent , public engine::Object
    {
    public:
        
        std::string queue;                  // Name of the queue we are recovering
        std::string fileName;               // Name of the file to create
        
        std::vector<size_t> workers;        // Workers involved in the operation
        size_t num_finish_worker;              // Number of workers that have confirmed that everything has been sent
        
        int num_write_operations;           // Number of pending local write operations
        
        au::CounterCollection<int> counter_per_worker;     // Count the number of outputs per worker
        
        bool force_flag;                    // Flag to remove previos directory
        bool show_flag;
        
        PopDelilahComponent( std::string _queue , std::string _fileName , bool force_flag , bool show_flag );
        
        ~PopDelilahComponent();
        
        void run();
        
        // Function to receive packets
		void receive( Packet* packet );
 
		// Function to get the status
		std::string getStatus();

        // Function to send a notification to this object
        void notify( engine::Notification* notification );
    
        void check();
        
    };
    
	
	
}

#endif

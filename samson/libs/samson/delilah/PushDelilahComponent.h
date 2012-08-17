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
#include "au/containers/Uint64Vector.h"

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
    
	public:
    
		// Constructor
		PushDelilahComponent( DataSource * _dataSource , const std::vector<std::string>& queues );
		~PushDelilahComponent();
    
    // Function to start running
    void run();
    
    // engine::Object
    virtual void notify( engine::Notification* notification );
    
    // Function to receive packets
		void receive( const PacketPointer& packet ){} // No packet is received here any more
    
		// Function to get the status
		std::string getStatus();		
    
    // Virtual in DelilahComponent
    std::string getShortDescription();
    
  private:
    
    std::string current_status_; // Description of the current status
    
    // Main function executed by background thread
    void run_in_background();
    
    DataSource *data_source_;        // Source of data
    au::StringVector queues_;	       // Name of the stream-queue we are uploading
    
		size_t sent_size_;			         // Total size ( bytes ) sent to the system
		size_t uploaded_size_;		       // Total size ( bytes ) updloaded
    
    au::Uint64Set push_ids_;         // Set of push_id's waiting to be confirmed
    
    bool finish_process;             // Flag to indicate that we have process all input data
    
    au::Cronometer cronometer;
		
    au::Token token; // Mutex protection
    
    // Friend function executed by backgrount thread
    friend  void* run_PushDelilahComponent( void* p);
    
  };
	
	
}

#endif

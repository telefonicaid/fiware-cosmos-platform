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

		std::string queue;				// Name of the stream-queue we are uploading
        
		// Sumary information
        
		size_t totalSize;				// Total size to be uploaded ( all files )
		size_t processedSize;			// total size processed locally ( compressed and squeduled to the network )
		size_t uploadedSize;			// Total size of uploaded files

        bool finish_process;            // Flag to indicate that we have process all input data
        
        au::Cronometer cronometer;
        
		// Worker to send the next packet
		int worker;
		
        // Input txt files
        TXTFileSet fileSet;				
        
	public:

		// Error log ( public since it is access from delilah )
		au::ErrorManager error;
		
		
		PushComponent( std::vector<std::string> &fileNames , std::string _queue  );		
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
        
        network::QueueChannel *qc;          // Information about the queue we are recovering
        std::string fileName;               // Name of the file to create
        
        std::string parserOut;              // Operation used to transform binary data into txt
        
        int num_workers;                    // Number of workers ( where we have send the request )
        int num_finish_worker;              // Number of workers that have confirmed that everything has been sent
        
        int num_write_operations;           // Number of pending local write operations
        
        
		// Error log ( public since it is access from delilah )
		au::ErrorManager error;
        
        PopComponent( std::string queue , int channel , std::string _parserOut, std::string _fileName ) : DelilahComponent( DelilahComponent::pop )
        {
            qc = new network::QueueChannel();
            qc->set_queue(queue);
            qc->set_channel( channel );

            parserOut = _parserOut;
            fileName = _fileName;
            
            num_write_operations = 0;
            
        }
        
        ~PopComponent()
        {
            delete qc;
        }
        
        void run()
        {
            
            // Remove the output file
            engine::DiskOperation *operation = engine::DiskOperation::newRemoveOperation(fileName, 0); 
            engine::DiskManager::shared()->add( operation );                
            
            // Send to all the workers a message to pop a queue
            num_workers = delilah->network->getNumWorkers();
            num_finish_worker = 0;
            
            for ( int w = 0 ; w < num_workers ; w++ )
            {
                Packet *p = new Packet( Message::PopQueue );
                
                network::PopQueue *pq = p->message->mutable_pop_queue();
                
                pq->mutable_target()->CopyFrom( *qc );       // Target queue channel
                pq->set_parserout( parserOut );             // Name of the operation to parseOut content
                
                
                p->message->set_delilah_id(id);             // Identifier of the component at this delilah
                
                delilah->network->sendToWorker( w, p);
            }
            
        }
        
        // Function to receive packets
		void receive(int fromId, Message::MessageCode msgCode, Packet* packet);
 
        std::string getShortStatus()
        {
            std::string queue = qc->queue();
            int channel = qc->channel();
            
            if ( error.isActivated() )
            {
                return au::Format::string("Poppping from queue %s:%d to file %s. ERROR: %s" , queue.c_str() , channel ,fileName.c_str(), error.getMessage().c_str() );
            }
            else
                return au::Format::string("Poppping from queue %s:%d to file %s. Completed %d / %d workers" , queue.c_str() , channel ,fileName.c_str(), num_finish_worker , num_workers );
        }
        
		// Function to get the status
		std::string getStatus()
        {
            std::string queue = qc->queue();
            int channel = qc->channel();
            return au::Format::string("Poppping from queue %s:%d to file %s. Completed %d / %d workers" , queue.c_str() , channel ,fileName.c_str(), num_finish_worker , num_workers );
        }

        // Function to send a notification to this object
        void notify( engine::Notification* notification );
    
        void check();
        
    };
    
	
	
}

#endif

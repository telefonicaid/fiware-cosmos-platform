

#include "PushComponent.h"                      // Own interface
#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/Buffer.h"							// samson::Buffer
#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...

namespace samson
{

	PushComponent::PushComponent( std::vector<std::string> &fileNames , std::string _queue  ) : 
            DelilahComponent(DelilahComponent::push) , fileSet( fileNames ) 
	{
		
		// Queue name 
		queue = _queue;

		uploadedSize = 0;
		processedSize = 0;
		totalSize = 0;	
		
		// Compute the total size for all the files
		for ( size_t i =  0 ; i < fileNames.size() ; i++)
			totalSize += au::Format::sizeOfFile( fileNames[i] );
        
        
        // Add myself as a receiver for notifications
        engine::Engine::add( notification_memory_request_response , this );
        
		
	}	
    
    void PushComponent::run()
    {
        
        // Get the number of workers of the system
        num_workers = delilah->network->getNumWorkers();
        worker = rand()%num_workers;
       
		if( totalSize == 0)
		{
			error.set("Not data to upload.");
            component_finished = true;
		}
        else
        {
            // Request the first buffer of memory
            requestMemoryBuffer();
        }
        
    }
    
	
	PushComponent::~PushComponent()
	{
        engine::Engine::remove( this );
	}
	
    
    // Request a memory buffer to upload the next packet...
    
    void PushComponent::requestMemoryBuffer()
    {
        // Ask for a memory buffer of 64 Mb
        engine::Notification *notification = new engine::Notification( notification_memory_request );
        notification->environment.setSizeT( "size", 64*1024*1024 );
        notification->environment.set("target", "PushComponent");
        notification->environment.setSizeT("id", id);
        engine::Engine::add( notification );
    }

    // Receive packets
    void PushComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
    {
        // At the moment we do not receive anything
    }

    
    // Notifications
    
    void PushComponent::notify( engine::Notification* notification )
    {
        if( notification->isName( notification_memory_request_response ) )
        {
            // New buffer to be used to send data to the workers
            engine::Buffer *buffer = (engine::Buffer *) notification->extractObject();
            if( !buffer )
                LM_X(1, ("Internal error: Memory request returnes without a buffer"));
            
            // Full the buffer with the content from the files
            fileSet.fill( buffer );
            
            // Get the size to update the total process info
            processedSize += buffer->getSize();
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_queue( queue );
            
            delilah->network->send(delilah, delilah->network->workerGetIdentifier(worker), packet);
            worker++;
            if( worker == num_workers )
                worker = 0;
            
            
            if( !fileSet.isFinish() )
                requestMemoryBuffer();  // Request the next element
            else
                component_finished = true;  // Mark as finished
            
        }
        
    }
    
    bool PushComponent::acceptNotification( engine::Notification* notification )
    {
        if( notification->environment.get("target", "") == "PushComponent" )
            if( notification->environment.getSizeT("id", 0) == id )
                return true;
        return false;
    }

    
    std::string PushComponent::getStatus()
    {
        std::ostringstream output;
        output << "Pushing " << au::Format::string( totalSize , "Bytes" ) << " to queue " << queue;
        output << " ( " << au::Format::percentage_string(processedSize, totalSize) << " ) "; 
        return output.str();
    }

    
	
	
}

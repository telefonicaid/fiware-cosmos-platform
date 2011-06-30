


#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/MemoryRequest.h"
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"							// samson::Buffer
#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...

#include "PushComponent.h"                      // Own interface


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
        
        
        // Set this to false ( true will be the end of processing data )
        finish_process = false;
        
		
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
            finish_process = true;
            delilah->pushConfirmation(this);
		}
        else
        {
            // Request the first buffer of memory
            requestMemoryBuffer();
        }
        
    }
    
	
	PushComponent::~PushComponent()
	{
	}
	
    
    // Request a memory buffer to upload the next packet...
    
    void PushComponent::requestMemoryBuffer()
    {
        // Add a memory request to be responded to me
        engine::MemoryManager::shared()->add( new engine::MemoryRequest( 64*1024*1024 , getEngineId() ) );
    }

    // Receive packets
    void PushComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
    {
        // At the moment we do not receive anything
        if( packet->msgCode != Message::PushBlockResponse )
            LM_X(1, ("Received an unexpected packet in a push block operation"));
        
        uploadedSize += packet->message->push_block_response().request().size();
        
        if( finish_process )
        {
            if( totalSize == uploadedSize )
            {
                // Set this flag to indicate that the process has finished
                finish_process = true;
                delilah->pushConfirmation(this);
            }
        }
    }

    
    // Notifications
    
    void PushComponent::notify( engine::Notification* notification )
    {
        if( notification->isName( notification_memory_request_response ) )
        {
            // New buffer to be used to send data to the workers
            engine::MemoryRequest *memoryRequest = (engine::MemoryRequest *) notification->extractObject();

            if( !memoryRequest )
                LM_X(1, ("Internal error: Memory request returnes without a buffer"));

            if ( !memoryRequest->buffer )
                LM_X(1, ("Memory request returned without the allocated buffer"));
            
            engine::Buffer *buffer = memoryRequest->buffer;
            delete memoryRequest;
            
            
            // Full the buffer with the content from the files
            fileSet.fill( buffer );
            
            // Get the size to update the total process info
            processedSize += buffer->getSize();
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( id );

            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_txt(true);  // This is always txt blocks

            // Unique target
            network::QueueChannel *target = pb->add_target();
            target->set_queue( queue );
            target->set_channel( 0 );
            
            pb->set_size( buffer->getSize() );
            
            delilah->network->sendToWorker( worker, packet);
            worker++;
            if( worker == num_workers )
                worker = 0;
            
            
            if( !fileSet.isFinish() )
                requestMemoryBuffer();  // Request the next element
            else
                finish_process = true;  // Mark as finished
            
        }
        
    }
    

    
    std::string PushComponent::getStatus()
    {
        std::ostringstream output;
        output << "Pushing " << au::Format::string( totalSize , "Bytes" ) << " to queue " << queue;
        output << " ( " << au::Format::percentage_string(processedSize, totalSize) << " ) "; 
        return output.str();
    }

    
	
	
}

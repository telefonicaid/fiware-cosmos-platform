

#include <sys/stat.h>		// mkdir

#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/MemoryRequest.h"
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"							// engine::Buffer
#include "engine/Notification.h"                    // engine::Notificaiton

#include "samson/network/Packet.h"							// samson::Packet
#include "samson/network/Message.h"						// samson::Message
#include "samson/delilah/Delilah.h"						// samson::Delilah
#include "samson/common/samson.pb.h"						// network::...
#include "DelilahClient.h"					// samson::DelilahClient
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...

#include "PushDelilahComponent.h"                      // Own interface


namespace samson
{

	PushDelilahComponent::PushDelilahComponent( DataSource * _dataSource , std::string _queue  ) : DelilahComponent( DelilahComponent::push ) 
	{
		
		// Queue name 
		queues.insert( _queue );

        // Data source
        dataSource = _dataSource;
        
		uploadedSize = 0;
		processedSize = 0;
        totalSize = dataSource->getTotalSize();
        
        // Set this to false ( true will be the end of processing data )
        finish_process = false;
		
        setConcept( au::str("Pushing %s to queue/s %s" , au::str( dataSource->getTotalSize() , "bytes").c_str() , _queue.c_str() ) );
	}	
    
    void PushDelilahComponent::addQueue( std::string  _queue )
    {
		queues.insert( _queue );
    }
    
    void PushDelilahComponent::run()
    {
        
        // Get the number of workers of the system
        num_workers = delilah->network->getNumWorkers();
        worker = rand()%num_workers;
       
		if( totalSize == 0)
		{
			error.set("Not data to upload.");
            finish_process = true;
            setComponentFinished();
		}
        else
        {
            // Request the first buffer of memory
            requestMemoryBuffer();
        }
        
    }
	
	PushDelilahComponent::~PushDelilahComponent()
	{
        delete dataSource;
	}
    
    // Request a memory buffer to upload the next packet...
    
    void PushDelilahComponent::requestMemoryBuffer()
    {
        // Add a memory request to be responded to me
        engine::MemoryManager::shared()->add( new engine::MemoryRequest( 64*1024*1024 , getEngineId() ) );
    }

    // Receive packets
    void PushDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
    {
        // At the moment we do not receive anything
        if( packet->msgCode != Message::PushBlockResponse )
            LM_X(1, ("Received an unexpected packet in a push block operation"));
        
        uploadedSize += packet->message->push_block_response().request().size();
        
        if( totalSize > 0 )
            setProgress((double) uploadedSize / (double) totalSize );
        
        if( finish_process )
        {
            
            if( totalSize == uploadedSize )
            {
                // Set this flag to indicate that the process has finished
                finish_process = true;
                setComponentFinished();
            }
        }
        
        
    }

    
    // Notifications
    
    void PushDelilahComponent::notify( engine::Notification* notification )
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

            // Skip to write the header at the end
            buffer->skipWrite( sizeof(KVHeader) );
            
            // Full the buffer with the content from the files
            dataSource->fill( buffer );

            // Set the header    
            KVHeader *header = (KVHeader*) buffer->getData();
            header->initForTxt( buffer->getSize() - sizeof(KVHeader) );

            
            // Get the size to update the total process info
            processedSize += buffer->getSize();
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( id );

            network::PushBlock* pb =  packet->message->mutable_push_block();

            // Add queue
            std::set<std::string>::iterator q;
            for (q = queues.begin() ; q != queues.end() ; q++)
                pb->add_queue( *q );
            
            pb->set_size( buffer->getSize() - sizeof(KVHeader) );
            
            delilah->network->sendToWorker( worker, packet);
            worker++;
            if( worker == num_workers )
                worker = 0;
            
            
            if( !dataSource->isFinish() )
                requestMemoryBuffer();  // Request the next element
            else
                finish_process = true;  // Mark as finished
            
        }
        
    }
    

    
    std::string PushDelilahComponent::getStatus()
    {
        std::ostringstream output;
        output << " ( Processed " << au::percentage_string(processedSize, totalSize) << " )\n"; 
        output << " ( Uploaded  " << au::percentage_string(uploadedSize, totalSize) << " )\n"; 
        return output.str();
    }
    
}

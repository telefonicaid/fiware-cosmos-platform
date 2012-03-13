

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
#include "samson/common/SamsonSetup.h"					// samson::SamsonSetup
#include "samson/common/KVHeader.h"
#include "samson/common/MemoryTags.h"                     // samson::MemoryInput , samson::MemoryOutput...

#include "PushDelilahComponent.h"                      // Own interface

#include "DelilahClient.h"					// samson::DelilahClient

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
		
        setConcept( 
           au::str("Pushing %s to queue/s %s" , au::str( totalSize , "bytes").c_str() , _queue.c_str() ) 
                   );
	}	
    
    std::string PushDelilahComponent::getShortDescription()
    {
        if( isComponentFinished() )
        {
            std::ostringstream output;
            output << "[ ";
            output << "Id " << id << " ";
            output << "Finished ";
            output << "]";
            return output.str();
            
        }
        
        
        std::ostringstream output;
        output << "[ ";
        output << "Id " << id << " ";
        output << "Pushed ";
        output << au::str( processedSize , "B" ) << " / " << au::str( totalSize , "B" );
        output << "]";
        return output.str();
    }
    
    
    void PushDelilahComponent::addQueue( std::string  _queue )
    {
		queues.insert( _queue );
    }
    
    void PushDelilahComponent::run()
    {
       
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
    	//LM_M(("PushDelilahComponent::requestMemoryBuffer: Request size:%lu by %lu", 64*1024*1024, getEngineId()));
        engine::MemoryManager::shared()->add( new engine::MemoryRequest( 64*1024*1024 , 1.0 , getEngineId() ) );
    }

    // Receive packets
    void PushDelilahComponent::receive( Packet* packet )
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
        	//LM_M(("PushDelilahComponent::notify: Received notification from memory request"));
            engine::MemoryRequest *memoryRequest = (engine::MemoryRequest *) notification->extractObject();

            if( !memoryRequest )
                LM_X(1, ("Internal error: Memory request returns without a buffer"));

            
            if ( !memoryRequest->buffer )
            {
                setComponentFinishedWithError( "Memory request returned without the allocated buffer" );
                return;
            }
            
            engine::Buffer *buffer = memoryRequest->buffer;
            delete memoryRequest;

            // Skip to write the header at the end
            buffer->skipWrite( sizeof(KVHeader) );
            
            // Full the buffer with the content from the files
            if( dataSource->fill( buffer ) != 0)
            {
                setComponentFinishedWithError("Error filling buffer");
                return;
            }

            // Set the header    
            KVHeader *header = (KVHeader*) buffer->getData();
            header->initForTxt( buffer->getSize() - sizeof(KVHeader) );

            
            // Get the size to update the total process info
            processedSize += buffer->getSize();
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_component_id( id );

            network::PushBlock* pb =  packet->message->mutable_push_block();

            // Add queue
            std::set<std::string>::iterator q;
            for (q = queues.begin() ; q != queues.end() ; q++)
                pb->add_queue( *q );
            
            pb->set_size( buffer->getSize() - sizeof(KVHeader) );
            
            // Get the next worker_id to send data...
            size_t worker_id = delilah->getNextWorkerId();
            
            // Information about destination
            packet->to.node_type = WorkerNode;
            packet->to.id = worker_id;
            
            // Send packet
            delilah->network->send( packet );
            
            if( !dataSource->isFinish() )
            {
            	LM_M(("PushDelilahComponent::notify: Request a new MemoryBuffer"));
                requestMemoryBuffer();  // Request the next element
            }
            else
                finish_process = true;  // Mark as finished
            
        }
        else
        {
        	LM_M(("PushDelilahComponent::notify: Received another type of notification"));
        }
        
    }
    

    
    std::string PushDelilahComponent::getStatus()
    {
        
        std::ostringstream output;

        au::tables::Table table( au::StringVector("Concept" , "Size" , "Percentadge" ) );

        au::StringVector values;

        {
            values.push_back("Pushed");
            values.push_back(au::str( processedSize , "B" ));
            values.push_back(au::str_percentage(processedSize, totalSize));              
            table.addRow(values);
        }

        {
            values.clear();
            values.push_back("Confirmed Pushed");
            values.push_back(au::str( uploadedSize , "B" ));
            values.push_back(au::str_percentage(uploadedSize, totalSize));              
            table.addRow(values);
        }
        
        output << table.str(concept);                        
        return output.str();
    }
    
}



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

#include "PushComponent.h"                      // Own interface


namespace samson
{

	PushComponent::PushComponent( DataSource * _dataSource , std::string _queue  ) : DelilahComponent( DelilahComponent::push ) 
	{
		
		// Queue name 
		queues.insert( _queue );

        // Data source
        dataSource = _dataSource;
        
		uploadedSize = 0;
		processedSize = 0;
		totalSize = 0;	
		

        totalSize = dataSource->getTotalSize();
        
        // Set this to false ( true will be the end of processing data )
        finish_process = false;
		
	}	
    
    void PushComponent::addQueue( std::string  _queue )
    {
		queues.insert( _queue );
    }
    
    void PushComponent::run()
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
	
	PushComponent::~PushComponent()
	{
        delete dataSource;
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
                setComponentFinished();
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
            dataSource->fill( buffer );
            
            // Get the size to update the total process info
            processedSize += buffer->getSize();
            
            Packet* packet = new Packet( Message::PushBlock );
            packet->buffer = buffer;    // Set the buffer of data
            packet->message->set_delilah_id( id );

            network::PushBlock* pb =  packet->message->mutable_push_block();
            pb->set_txt(true);  // This is always txt blocks

            // Add queue
            std::set<std::string>::iterator q;
            for (q = queues.begin() ; q != queues.end() ; q++)
                pb->add_queue( *q );
            
            pb->set_size( buffer->getSize() );
            
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
    

    
    std::string PushComponent::getStatus()
    {
        std::ostringstream output;
        output << "Pushing " << au::str( totalSize , "Bytes" ) << " to queues ";
        
        std::set<std::string>::iterator q;
        for (q = queues.begin() ; q != queues.end() ; q++)
            output << *q;
        
        output << " ( Processed " << au::Format::percentage_string(processedSize, totalSize) << " ) "; 
        output << " ( Uploaded  " << au::Format::percentage_string(uploadedSize, totalSize) << " ) "; 
        return output.str();
    }
    
    std::string PushComponent::getShortStatus()
    {

        std::ostringstream output;
        output << "Pushing " << au::str( totalSize , "Bytes" ) << " to queues ";
        
        std::set<std::string>::iterator q;
        for (q = queues.begin() ; q != queues.end() ; q++)
            output << *q;
        
        output << " ( Processed " << au::Format::percentage_string(processedSize, totalSize) << " ) "; 
        output << " ( Uploaded  " << au::Format::percentage_string(uploadedSize, totalSize) << " ) "; 
        return output.str();
    }
    
    
    
#pragma mark pop
    
    PopComponent::PopComponent( std::string _queue , std::string _state , std::string _fileName , bool _force_flag ) : DelilahComponent( DelilahComponent::pop )
    {
        
        LM_M(("POP Component "));
        
        queue = _queue;
        state = _state;
        
        fileName = _fileName;
        
        num_write_operations = 0;
        
        num_outputs = 0;

        force_flag = _force_flag;
        
    }
    
    PopComponent::~PopComponent()
    {
    }
    
    std::string PopComponent::getShortStatus()
    {
        
        std::ostringstream message;
        
        message << "Popping from ";
        
        if( queue.length() > 0 )
            message << "queue " << queue;
        else if ( state.length() > 0 )
            message << "state " << state;
        
        if ( error.isActivated() )
            message << " ( " << error.getMessage() << " ) ";
        else
            message << " ( Completed " << num_finish_worker << " / " << num_workers << " workers )";
        
        return message.str();
    }
    
    // Function to get the status
    std::string PopComponent::getStatus()
    {
        return getShortStatus();
    }
    
    
    void PopComponent::run()
    {

        if( force_flag )
            au::removeDirectory( fileName );
        
        if( mkdir( fileName.c_str() , 0755 ) )
        {
            setComponentFinishedWithError( au::str( "Not possible to create directory %s." , fileName.c_str() ) );
            return;
        }
                
        // Send to all the workers a message to pop a queue
        num_workers = delilah->network->getNumWorkers();
        num_finish_worker = 0;
        
        for ( int w = 0 ; w < num_workers ; w++ )
        {
            Packet *p = new Packet( Message::PopQueue );
            
            network::PopQueue *pq = p->message->mutable_pop_queue();
            
            if( queue.length() > 0 )
                pq->set_queue( queue );                     // Set the name of the queue
            else if( state.length() > 0 )
                pq->set_state( state );                     // Set the name of the queue
            
            p->message->set_delilah_id(id);             // Identifier of the component at this delilah
            
            delilah->network->sendToWorker( w, p);
        }
        
    }
    
    void PopComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
    {
        if( msgCode != Message::PopQueueResponse )
        {
            delete packet;
            return;
        }
        
        if( packet->buffer )
        {
            
            num_write_operations++;
         
            std::string _fileName = au::str("%s/file_%d" , fileName.c_str() , num_outputs++ );
            
            engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation( packet->buffer , _fileName , getEngineId() );
            engine::DiskManager::shared()->add( operation );                
            
        }
        
        // If finished,
        if( packet->message->pop_queue_response().finish() )
            num_finish_worker++;
        
        // Check errors
        if( packet->message->pop_queue_response().has_error() )
            error.set( packet->message->pop_queue_response().error().message() );
        

        check();
    }
    
    
    void PopComponent::notify( engine::Notification* notification )
    {
        if( notification->isName(notification_disk_operation_request_response) )
        {
            LM_M(("Received a disk operation notification"));
            num_write_operations--;
            check();
        }
        else
            LM_W(("Unexpected notification %s" , notification->getName() ));
            
    }
    
    void PopComponent::check()
    {
        if ( error.isActivated() )
            setComponentFinished();
        
        if( ( num_finish_worker == num_workers ) && (num_write_operations==0) )
        {
            // Finish operation
            setComponentFinished();
        }
        
    }

    
	
	
}

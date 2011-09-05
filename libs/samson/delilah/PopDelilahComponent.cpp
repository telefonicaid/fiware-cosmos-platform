

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

#include "PopDelilahComponent.h"                      // Own interface


namespace samson
{
    
    
#pragma mark pop
    
    PopDelilahComponent::PopDelilahComponent( std::string _queue , std::string _fileName , bool _force_flag ) : DelilahComponent( DelilahComponent::pop )
    {
        
        queue = _queue;
        
        fileName = _fileName;
        
        num_write_operations = 0;
        
        num_outputs = 0;

        force_flag = _force_flag;
        
        setConcept( au::str("Pop queue %s to local directory %s" , queue.c_str() , fileName.c_str() ) );
    }
    
    PopDelilahComponent::~PopDelilahComponent()
    {
    }
    
    // Function to get the status
    std::string PopDelilahComponent::getStatus()
    {
        std::ostringstream message;
        
        if ( error.isActivated() )
        {
            // Nothing since error is shown by the delilah
        }
        else
            message << "Completed " << num_finish_worker << " / " << num_workers << " workers";
        
        return message.str();
    }
    
    
    void PopDelilahComponent::run()
    {

        if( force_flag )
        {
            au::ErrorManager error;
            au::removeDirectory( fileName , error );
            if( error.isActivated() )
                delilah->showWarningMessage( error.getMessage() );
        }
        
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
            
            pq->set_queue( queue );                     // Set the name of the queue
            
            p->message->set_delilah_id(id);             // Identifier of the component at this delilah
            
            delilah->network->sendToWorker( w, p);
        }
        
    }
    
    void PopDelilahComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
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
        
        if( num_workers > 0 )
            setProgress( (double) num_finish_worker /  (double) num_workers );
        
        // Check errors
        if( packet->message->pop_queue_response().has_error() )
            error.set( packet->message->pop_queue_response().error().message() );
        

        check();
    }
    
    
    void PopDelilahComponent::notify( engine::Notification* notification )
    {
        if( notification->isName(notification_disk_operation_request_response) )
        {
            num_write_operations--;
            check();
        }
        else
            LM_W(("Unexpected notification %s" , notification->getName() ));
            
    }
    
    void PopDelilahComponent::check()
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

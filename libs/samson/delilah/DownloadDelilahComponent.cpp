
#include <sys/stat.h>		// mkdir

#include "au/file.h"

#include "engine/MemoryManager.h"					// samson::MemoryManager
#include "engine/MemoryRequest.h"
#include "engine/Notification.h"                // engine::Notification

#include "engine/Buffer.h"							// engine::Buffer
#include "engine/Notification.h"                    // engine::Notificaiton

#include "engine/DiskOperation.h"                   // engine::DiskOperation
#include "engine/DiskManager.h"                     // endinge::DiskManager

#include "DownloadDelilahComponent.h"
#include "samson/delilah/Delilah.h"				// samson::Delilah

namespace samson {

	
	DelilahDownloadComponent::DelilahDownloadComponent( std::string _queue , std::string _fileName , bool _force_flag ) 
        : DelilahComponent(DelilahComponent::load)
	{
		queue = _queue;
		fileName = _fileName;
        force_flag = _force_flag;

        // We still have not received init response
		received_init_response = false;
        
		// Initial state until we receive information about queue
		num_files_to_download = -1;
		num_files_downloaded = 0;
		
        // Counter of write operation ( to wait for them before seting this component as finished )
        num_write_operations = 0;
        
        setConcept(au::str("Downloading data-set %s to local directory %s", queue.c_str() , fileName.c_str() ));
	}
	
	std::string DelilahDownloadComponent::getDescription()
	{
		std::ostringstream o;
		o << "Downloading from queue " << queue << " to local file " << fileName;
		return o.str();
	}


	void DelilahDownloadComponent::run()
	{
        // Create firectory    
        if( force_flag )
        {
            au::ErrorManager error;
            au::removeDirectory( fileName , error );
            if( error.isActivated() )
                delilah->showWarningMessage( error.getMessage() );
        }
		
		// Send the message to the controller
		Packet *p = new Packet(Message::DownloadDataInit);
		p->message->set_delilah_id( id );
		samson::network::DownloadDataInit *download_data_init = p->message->mutable_download_data_init();
		download_data_init->set_queue( queue );	// Set the queue we want to download
		delilah->network->sendToController( p );
		
	}
	
	void DelilahDownloadComponent::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		
		if (msgCode == Message::DownloadDataInitResponse )
		{
            received_init_response = true;
            
            const network::DownloadDataInitResponse & download_data_init_response = packet->message->download_data_init_response();
            
            // Error in the init response
			if( download_data_init_response.has_error() )
			{
				error.set( download_data_init_response.error().message() );
                //LM_M(("download: setComponentFinished() with error message"));
                setComponentFinished();
				return;
			}
			
			// Set the number of files to download
			num_files_to_download = download_data_init_response.queue().file_size();
            
            if( num_files_to_download == 0)
            {
                // Nothing to be downloaded
                //LM_M(("download: setComponentFinished() with Nothing to be downloaded"));
                setComponentFinished();
            }
            else
            {
                if( mkdir( fileName.c_str() , 0755 ) )
                    setComponentFinishedWithError( au::str( "Not possible to create directory %s (%s)." , fileName.c_str() , strerror(errno)) );
                
            }
            
            // Send packet to each worker asking for the associated files
			for ( int i = 0 ; i < download_data_init_response.queue().file_size() ; i++)
			{
				Packet *p = new Packet( Message::DownloadDataFile);
				p->message->set_delilah_id( id );
                
				samson::network::DownloadDataFile *download_data_file = p->message->mutable_download_data_file();
                
                // Set the load id ( at controller )
                download_data_file->set_load_id( download_data_init_response.load_id() );
                
                // Order in witch the files will be downloaded
                download_data_file->set_file_id( i );
                
				// Copy the file information
				download_data_file->mutable_file()->CopyFrom( download_data_init_response.queue().file(i) );
				
				// Send to the rigth worker
				int worker = download_data_init_response.queue().file(i).worker();
                
				delilah->network->sendToWorker( worker , p);
			}
			
		}
		else if (msgCode == Message::DownloadDataFileResponse)
		{
			
            // Increase the counter of number of files already downloaded
			num_files_downloaded ++;
			
			// Create a disk-write operation to save this buffer 
            num_write_operations++;
            
            int worker_id = delilah->network->getWorkerFromIdentifier( fromId );
            int num_output = counter_per_worker.getCounterFor( worker_id );
            
            std::string _fileName = au::str("%s/worker_%06d_file_%06d" , fileName.c_str() , worker_id, num_output );
            engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation( packet->buffer , _fileName , getEngineId() );
            engine::DiskManager::shared()->add( operation );                
			
		}
		
	}

    void DelilahDownloadComponent::notify( engine::Notification* notification )
    {
        if( notification->isName(notification_disk_operation_request_response) )
        {
            num_write_operations--;
            check();
        }
        else
            LM_W(("Unexpected notification %s" , notification->getName() ));
        
    }
	
    void DelilahDownloadComponent::check()
    {
        if( !received_init_response )
            return;
        
        if( num_files_to_download == num_files_downloaded )
            if ( num_write_operations == 0 )
		{
            //LM_M(("download: setComponentFinished() with ( num_write_operations == 0 )"));
                setComponentFinished();
		}
    }
    
	std::string DelilahDownloadComponent::getStatus()
	{
		std::ostringstream output;
		output << "[ "<< id << " ] Downloading queue " << queue << " to " << fileName << " || ";

		if( !received_init_response )
            output << " Waiting controller response";
        else
        {
            if( num_files_downloaded == num_files_to_download )
                output << " ( All files downloaded )";
            else
                output << " ( Downloaded: " << num_files_downloaded << "/" << num_files_to_download << " files ) ";
            
            if( num_write_operations > 0 )
                output << " ( Pending " << num_write_operations << " local-writes operations )"; 
        }
		return output.str();
	}
	
	std::string DelilahDownloadComponent::getShortStatus()
	{
        return getStatus();

	}    
}

#include "engine/Buffer.h"					// ss::Buffer
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet
#include "engine/Buffer.h"					// ss::Buffer
#include "LoadDataManager.h"		// Own interface
#include "MessagesOperations.h"		// setErrorMessage
#include "SamsonSetup.h"			// ss::SamsonSetup
#include "engine/DiskOperation.h"			// ss::DiskOperation
#include "engine/Engine.h"                 // ss::Engine
#include "engine/DiskManager.h"            // Notifications

#include "MemoryTags.h"                 // MemoryOuputNetwork

namespace ss
{
#pragma mark DataManagerItem
	
	LoadDataManagerItem::LoadDataManagerItem( size_t _id , size_t _load_id, int _fromIdentifier , LoadDataManager *_dataManager )
	{
		id = _id;
        load_id = _load_id;
		fromIdentifier = _fromIdentifier;
		dataManager = _dataManager;
	}
	


#pragma mark UploadItem
	
	UploadItem::UploadItem(size_t id, int _fromIdentifier , LoadDataManager *dataManager, const network::UploadDataFile &_upload_data_file ,size_t _sender_id, engine::Buffer * _buffer ) 
		: LoadDataManagerItem( id, _upload_data_file.load_id()  ,_fromIdentifier , dataManager)
	{
		upload_data_file = new network::UploadDataFile();
		upload_data_file->CopyFrom( _upload_data_file );	// Copy the message
		sender_id = _sender_id;

		buffer = _buffer;			// Point to the buffer
		
		size = buffer->getSize();	// Used size inside buffer

		// Select a name for the uploaded file
        
        int worker_id = dataManager->worker->network->getWorkerId();    // Get the worker id information
        
#ifdef __LP64__
        fileName = au::Format::string( "worker_%d_job_%lu_id_%lu.txt", worker_id, load_id, id);
#else
        fileName = au::Format::string( "worker_%d_job_%d_id_%d.txt", worker_id, load_id, id);
#endif

        
	}

	UploadItem::~UploadItem()
	{
		delete upload_data_file;
	}


	void UploadItem::submitToFileManager()
	{
		// Add to the file manager to be stored on disk
        engine::DiskOperation *operation = engine::DiskOperation::newWriteOperation( buffer , SamsonSetup::dataFile( fileName ) );

		// Submit to the engine
        engine::Notification *notification = new engine::Notification( notification_disk_operation_request , operation );
        notification->environment.set("target", "LoadDataManager");
        notification->environment.setSizeT("id", id);
        notification->environment.setInt("worker", dataManager->worker->network->getWorkerId());
        engine::Engine::add( notification);
	}

	void UploadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet( Message::UploadDataFileResponse );
		network::UploadDataFileResponse *response = p->message->mutable_upload_data_file_response();

		// Copy the original message
		response->mutable_query()->CopyFrom( *upload_data_file );
		
		// Set the delalilah identifier
		p->message->set_delilah_id( sender_id );
		
		// Set the file just created
		network::File *file = response->mutable_file();
		file->set_name( fileName );
		file->set_worker( dataManager->worker->network->getWorkerId());
		network::KVInfo *info = file->mutable_info();
		info->set_kvs(1);
		info->set_size(size);
		
		// Set the error int the response message if necessary
		if( error )
			response->mutable_error()->set_message( error_message );

		// Send the message to delilah
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier  , p);
	}

    
    // Function to get the run-time status of this object
    std::string UploadItem::getStatus( )
    {
        std::ostringstream output;
        output << "<Up:" << au::Format::string(size,"B") << ">";
        return output.str();
    }	
	
#pragma mark DownloadItem
	
	DownloadItem::DownloadItem(size_t id, int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadDataFile &_download_data_file ,size_t _sender_id) 
		: LoadDataManagerItem( id, _download_data_file.load_id() , _fromIdentifier , dataManager )
	{
		download_data_file = new network::DownloadDataFile();
		download_data_file->CopyFrom( _download_data_file );	// Copy the message
		sender_id = _sender_id;
		
	}
	
	DownloadItem::~DownloadItem()
	{
		delete download_data_file;
	}

	void DownloadItem::submitToFileManager()
	{
		std::string fileName = download_data_file->file().name();
		size_t size = au::Format::sizeOfFile( SamsonSetup::shared()->dataDirectory + "/" + fileName );

		buffer = engine::MemoryManager::shared()->newBuffer( "Buffer for downloading data" , size , MemoryOutputNetwork );
		buffer->setSize( size );

        engine::DiskOperation *operation = engine::DiskOperation::newReadOperation( buffer->getData() ,  SamsonSetup::dataFile(fileName) , 0 , size );
        
		// Submit the operation to the engine
        engine::Notification *notification = new engine::Notification( notification_disk_operation_request , operation );
        
        notification->environment.set("target", "LoadDataManager");
        notification->environment.setSizeT("id", id);
        notification->environment.setInt("worker", dataManager->worker->network->getWorkerId());
        
        // add something here to identify as yours
        engine::Engine::add( notification);
	}
	

	void DownloadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet( Message::DownloadDataFileResponse );
		network::DownloadDataFileResponse *response = p->message->mutable_download_data_file_response();

		// Copy the original message
		response->mutable_query()->CopyFrom( *download_data_file );
		
		// Set the delilah identifier
		p->message->set_delilah_id( sender_id );
		
		// Put the buffer where the file is loaded
		p->buffer = buffer;	// Put the buffer here

		// Set the rigth error if necessary
		if( error )
			response->mutable_error()->set_message( error_message );

		// Send the message
		
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier  , p);
	}	
	
#pragma mark LoadDataManager
	
	LoadDataManager::LoadDataManager( SamsonWorker *_worker )
	{
		worker = _worker;
		upload_size = 0;
        
        // Initial value for the id of the laod operations
        id = 1;
        
        // Add this object as a listener of notification_disk_operation_request_response
        engine::Engine::add( notification_disk_operation_request_response , this );
	}
	

    LoadDataManager::~LoadDataManager()
    {
        engine::Engine::remove( this );
    }
    
	void LoadDataManager::addUploadItem( int fromIdentifier, const network::UploadDataFile &uploadData ,size_t sender_id, engine::Buffer * buffer )
	{
		lock.lock();
		
		// Get the size of the upload buffer....
		upload_size += buffer->getSize();
		
		UploadItem *item = new UploadItem( id++ , fromIdentifier , this , uploadData , sender_id , buffer );
		uploadItem.insertInMap( item->id , item );
		
		item->submitToFileManager();

		lock.unlock();
	}
	


	void LoadDataManager::addDownloadItem( int fromIdentifier, const network::DownloadDataFile &downloadData , size_t sender_id )
	{
		lock.lock();
		
		DownloadItem *item = new DownloadItem(id++, fromIdentifier , this , downloadData, sender_id );
		downloadItem.insertInMap( item->id , item );

		item->submitToFileManager();
		
		lock.unlock();
		
	}
	
    void LoadDataManager::setNotificationCommonEnvironment( engine::Notification* notification )
    {
        notification->environment.set("target", "LoadDataManager" );
        notification->environment.setInt("worker", worker->network->getWorkerId());
    }
    
    bool LoadDataManager::acceptNotification( engine::Notification* notification )
    {
        if( notification->environment.get("target","") != "LoadDataManager" )
            return false;
        
        if( notification->environment.getInt("worker", -1) != worker->network->getWorkerId() )
            return false;
        
        return  true;
    }
    
    void LoadDataManager::notify( engine::Notification* notification )
    {
        if( !notification->isName( notification_disk_operation_request_response ) )
            LM_X(1,("LoadDataManager received a wrong notification"));
        
        if( !notification->containsObject() )
            LM_X(1,("LoadDataManager received a notification_disk_operation_request_response with a wrong number of parameters"));
        
        engine::DiskOperation *operation = (engine::DiskOperation*) notification->extractObject();
        
        size_t _id = notification->environment.getSizeT("id", 0);
        
        if( _id == 0)
            LM_W(("LoadDataManger received a notification_disk_operation_request_response without id field"));
        
        switch ( operation->getType() ) {
                
			case engine::DiskOperation::read:
			{
				DownloadItem* download = downloadItem.extractFromMap( _id );
				
				if( download )
				{
					download->sendResponse( operation->error.isActivated() , operation->error.getMessage() );
					delete download;
				}
				
			}
                
				break;
			case engine::DiskOperation::write:
			{
				UploadItem* upload = uploadItem.extractFromMap( _id );
				
				if( upload )
				{
					upload->sendResponse(operation->error.isActivated() , operation->error.getMessage());
					delete upload;
				}
				
			}
				break;
			case engine::DiskOperation::remove:
				break;
			default:
				break;
		}
        
        delete operation;
        
    }
	

	void LoadDataManager::fill( network::WorkerStatus* ws)
	{
		lock.lock();
		
		std::ostringstream output; 

		output << "Download: ";
		{
			au::map<size_t,DownloadItem>::iterator iter;
			for ( iter = downloadItem.begin() ; iter != downloadItem.end() ; iter++)
				output << iter->second->getStatus();
		}

		output << "Upload: ";
		{
			au::map<size_t,UploadItem>::iterator iter;
			for ( iter = uploadItem.begin() ; iter != uploadItem.end() ; iter++)
				output << iter->second->getStatus();
		}

		upload_size = 0;	// Reset the counter of upload bytes
		
		lock.unlock();
		
		ws->set_load_data_manager_status( output.str() );
	}
}

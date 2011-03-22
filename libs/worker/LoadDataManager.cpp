#include "Buffer.h"					// ss::Buffer
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet
#include "Buffer.h"					// ss::Buffer
#include "LoadDataManager.h"		// Own interface
#include "MessagesOperations.h"		// setErrorMessage
#include "SamsonSetup.h"			// ss::SamsonSetup
#include "DiskOperation.h"			// ss::DiskOperation

namespace ss
{
#pragma mark DataManagerItem
	
	LoadDataManagerItem::LoadDataManagerItem( size_t _id , int _fromIdentifier , LoadDataManager *_dataManager )
	{
		id = _id;
		fromIdentifier = _fromIdentifier;
		dataManager = _dataManager;
	}
	


#pragma mark UploadItem
	
	UploadItem::UploadItem(size_t id, int _fromIdentifier , LoadDataManager *dataManager, const network::UploadDataFile &_upload_data_file ,size_t _sender_id, Buffer * _buffer ) 
		: LoadDataManagerItem( id,  _fromIdentifier , dataManager)
	{
		upload_data_file = new network::UploadDataFile();
		upload_data_file->CopyFrom( _upload_data_file );	// Copy the message
		sender_id = _sender_id;

		buffer = _buffer;			// Point to the buffer
		
		fileName = newFileName();	// New file name for this upload
		size = buffer->getSize();	// Used size inside buffer
		
	}

	UploadItem::~UploadItem()
	{
		delete upload_data_file;
	}


	void UploadItem::submitToFileManager()
	{
		// Add to the file manager to be stored on disk
		DiskOperation *operation = DiskOperation::newWriteOperation( buffer , fileName , dataManager );
		operation->tag = id;	// Use my id as tag

		// Submit to the engine
		Engine::shared()->add( operation  );
	}

	void UploadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet();
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
		network->send( dataManager->worker , fromIdentifier , Message::UploadDataFileResponse , p);
	}
	
	
	
#pragma mark DownloadItem
	
	DownloadItem::DownloadItem(size_t id, int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadDataFile &_download_data_file ,size_t _sender_id) 
		: LoadDataManagerItem( id, _fromIdentifier , dataManager )
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

		buffer = Engine::shared()->memoryManager.newBuffer( "Buffer for downloading data" , size , Buffer::output );
		buffer->setSize( size );

		DiskOperation *operation = DiskOperation::newReadOperation( buffer->getData() , fileName , 0 , size , dataManager );
		operation->tag = id;	// Use my id as tag

		// Submit the operation to the engine
		Engine::shared()->add( operation );
	}
	

	void DownloadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet();
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
		network->send( dataManager->worker , fromIdentifier , Message::DownloadDataFileResponse , p);
	}	
	
#pragma mark LoadDataManager
	
	LoadDataManager::LoadDataManager( SamsonWorker *_worker )
	{
		worker = _worker;
		upload_size = 0;
	}
	

	void LoadDataManager::addUploadItem( int fromIdentifier, const network::UploadDataFile &uploadData ,size_t sender_id, Buffer * buffer )
	{
		lock.lock();
		
		// Get the size of the upload buffer....
		upload_size += buffer->getSize();
		
		UploadItem *item = new UploadItem( id++, fromIdentifier , this , uploadData , sender_id , buffer );
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
	
	void LoadDataManager::diskManagerNotifyFinish(  DiskOperation *operation )
	{
		
		switch (operation->getType()) {

			case DiskOperation::read:
			{
				DownloadItem* download = downloadItem.extractFromMap( operation->tag );
				
				if( download )
				{
					download->sendResponse( operation->error.isActivated() , operation->error.getMessage() );
					delete download;
				}
				
			}
		
				break;
			case DiskOperation::write:
			{
				UploadItem* upload = uploadItem.extractFromMap( operation->tag );
				
				if( upload )
				{
					upload->sendResponse(operation->error.isActivated() , operation->error.getMessage());
					delete upload;
				}
				
			}
				break;
			case DiskOperation::remove:
				break;
			default:
				break;
		}
		
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
		

		ws->set_upload_size( upload_size );
		
		upload_size = 0;	// Reset the counter of upload bytes
		
		lock.unlock();
		
		ws->set_load_data_manager_status( output.str() );
	}
}

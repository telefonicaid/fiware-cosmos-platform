#include "LoadDataManager.h"		// Own interface
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager
#include "FileManagerReadItem.h"
#include "FileManagerWriteItem.h"

namespace ss
{
	
	
#pragma mark DataManagerItem
	
	LoadDataManagerItem::LoadDataManagerItem( int _fromIdentifier , LoadDataManager *_dataManager )
	{
		fromIdentifier = _fromIdentifier;
		dataManager = _dataManager;
	}
	

#pragma mark UploadItem
	
	UploadItem::UploadItem( int _fromIdentifier , LoadDataManager *dataManager, const network::UploadData &_uploadData ,size_t _sender_id, Buffer * _buffer ) 
		: LoadDataManagerItem( _fromIdentifier , dataManager)
	{
		uploadData = _uploadData;	// Copy the message
		sender_id = _sender_id;

		buffer = _buffer;			// Point to the buffer
		
		fileName = newFileName();	// New file name for this upload
		size = buffer->getSize();	// Used size inside buffer
		
	}

	size_t UploadItem::submitToFileManager()
	{
		// Add to the file manager to be stored on disk
		FileManagerWriteItem * item = new FileManagerWriteItem( fileName , buffer , dataManager );
		return FileManager::shared()->addItemToWrite( item );
	}
	
	void UploadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet();
		network::UploadDataResponse *response = p->message.mutable_upload_data_response();
		response->mutable_upload_data()->CopyFrom( uploadData );
		p->message.set_delilah_id( sender_id );
		
		response->set_error( error );
		response->set_error_message( error_message );
		
		// Information about the created file
		network::File *file = response->mutable_file();
		file->set_name( fileName );
		file->set_worker( dataManager->worker->network->getWorkerId());
		network::KVInfo *info = file->mutable_info();
		info->set_kvs(1);
		info->set_size(size);
		
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier , Message::UploadDataResponse , p);
	}
	
	
	
#pragma mark DownloadItem
	
	DownloadItem::DownloadItem( int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadData &_downloadData ,size_t _sender_id) 
		: LoadDataManagerItem( _fromIdentifier , dataManager )
	{
		downloadData = _downloadData;	// Copy the message
		sender_id = _sender_id;
		
	}

	size_t DownloadItem::submitToFileManager()
	{
		std::string fileName = downloadData.file().name();
		size_t size = au::Format::sizeOfFile( fileName );

		buffer = MemoryManager::shared()->newBuffer( "Buffer for downloading data" , size );
		buffer->setSize( size );

		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , buffer->getSimpleBuffer(), dataManager );
		return FileManager::shared()->addItemToRead( item );
	}
	
	DownloadItem::~DownloadItem()
	{
	}
	
	void DownloadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet();
		network::DownloadDataResponse *response = p->message.mutable_download_data_response();
		response->mutable_download_data()->CopyFrom( downloadData );
		p->message.set_delilah_id( sender_id );
		p->buffer = buffer;	// Put the buffer here
		
		response->set_error( error );
		response->set_error_message( error_message );
		
		
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier , Message::DownloadDataResponse , p);
	}	
	
#pragma mark LoadDataManager
	
	void LoadDataManager::addUploadItem( int fromIdentifier, const network::UploadData &uploadData ,size_t sender_id, Buffer * buffer )
	{
		
		lock.lock();
		
		// Get the size of the upload buffer....
		upload_size += buffer->getSize();
		
		UploadItem *item = new UploadItem( fromIdentifier , this , uploadData , sender_id , buffer );
		
		size_t fm_id = item->submitToFileManager();

		//LM_M(("LDM Item (sender_id %d) (file_id %d) and was schedulled to Disk manager with id %d",sender_id,uploadData.file_id() ,fm_id));
		
		uploadItem.insertInMap( fm_id , item );
		
		lock.unlock();
	}
	
	void LoadDataManager::addDownloadItem( int fromIdentifier, const network::DownloadData &downloadData , size_t sender_id )
	{
		lock.lock();
		
		DownloadItem *item = new DownloadItem( fromIdentifier , this , downloadData, sender_id );

		size_t fm_id = item->submitToFileManager();
		
		downloadItem.insertInMap( fm_id , item );
		
		lock.unlock();
		
	}
	
	

	void LoadDataManager::fileManagerNotifyFinish(size_t fm_id, bool success)
	{
		//LM_M(("LDM File Manager finish with file manager id %d ( success %d ) ",fm_id,success));
		
		lock.lock();
		
		UploadItem* upload			= uploadItem.extractFromMap( fm_id );
		DownloadItem* download		= downloadItem.extractFromMap( fm_id );
		
		assert( download || upload );
		
		if( download )
		{
			download->sendResponse(!success ,"");
			delete download;
		}

		if( upload )
		{
			upload->sendResponse(!success ,"");
			delete upload;
		}
		
		
		lock.unlock();
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

		ws->set_upload_size( upload_size );
		
		lock.unlock();
		
		ws->set_load_data_manager_status( output.str() );
		

	}

	
	
	
}
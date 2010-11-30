#include "LoadDataManager.h"		// Own interface
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager

namespace ss
{
	
	
#pragma mark DataManagerItem
	
	DataManagerItem::DataManagerItem( int _fromIdentifier , LoadDataManager *_dataManager )
	{
		fromIdentifier = _fromIdentifier;
		dataManager = _dataManager;
	}
	

#pragma mark UploadItem
	
	UploadItem::UploadItem( int _fromIdentifier , LoadDataManager *dataManager, const network::UploadData &_uploadData , Buffer * _buffer ) 
		: DataManagerItem( _fromIdentifier , dataManager)
	{
		uploadData = _uploadData;	// Copy the message

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

		response->set_error( error );
		response->set_error_message( error_message );
		
		// Information about the created file
		network::File *file = response->mutable_file();
		file->set_name( fileName );
		file->set_worker( dataManager->worker->myWorkerId );
		network::KVInfo *info = file->mutable_info();
		info->set_kvs(1);
		info->set_size(size);
		
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier , Message::UploadDataResponse , p);
	}
	
	
	
#pragma mark DownloadItem
	
	DownloadItem::DownloadItem( int _fromIdentifier, LoadDataManager *dataManager, const network::DownloadData &_downloadData ) 
		: DataManagerItem( _fromIdentifier , dataManager )
	{
		downloadData = _downloadData;	// Copy the message
		
	}

	size_t DownloadItem::submitToFileManager()
	{
		std::string fileName = downloadData.file().name();
		size_t size = FileManagerReadItem::sizeOfFile( fileName );
		buffer = MemoryManager::shared()->newBuffer( "Buffer for downloading data" , size );
		
		FileManagerReadItem *item = new FileManagerReadItem( fileName , 0 , size , buffer->getData(), dataManager );
		return FileManager::shared()->addItemToRead( item );
	}
	
	DownloadItem::~DownloadItem()
	{
		MemoryManager::shared()->destroyBuffer( buffer );
	}
	
	void DownloadItem::sendResponse( bool error , std::string error_message )
	{
		// Sen a packet bak to delilah to confirm this update
		Packet *p = new Packet();
		network::DownloadDataResponse *response = p->message.mutable_download_data_response();
		response->mutable_download_data()->CopyFrom( downloadData );
		
		response->set_error( error );
		response->set_error_message( error_message );
		
		
		NetworkInterface *network = dataManager->worker->network;
		network->send( dataManager->worker , fromIdentifier , Message::DownloadDataResponse , p);
	}	
	
#pragma mark LoadDataManager
	
	void LoadDataManager::addUploadItem( int fromIdentifier, const network::UploadData &uploadData , Buffer * buffer )
	{
		lock.lock();
		
		UploadItem *item = new UploadItem( fromIdentifier , this , uploadData , buffer );
		
		size_t fm_id = item->submitToFileManager();
		
		uploadItem.insertInMap( fm_id , item );
		
		lock.unlock();
	}
	
	void LoadDataManager::addDownloadItem( int fromIdentifier, const network::DownloadData &downloadData )
	{
		lock.lock();
		
		DownloadItem *item = new DownloadItem( fromIdentifier , this , downloadData );

		size_t fm_id = item->submitToFileManager();
		
		downloadItem.insertInMap( fm_id , item );
		
		lock.unlock();
		
	}
	
	

	void LoadDataManager::fileManagerNotifyFinish(size_t fm_id, bool success)
	{
		
		lock.lock();
		
		UploadItem* upload			= uploadItem.extractFromMap( fm_id );
		DownloadItem* download		= downloadItem.extractFromMap( fm_id );
		
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
	
}
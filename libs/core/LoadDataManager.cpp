#include "LoadDataManager.h"		// Own interface
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet

namespace ss
{

	void LoadDataManager::addFile( Buffer * buffer , std::string fileName, int fromIdentifier , size_t process_id , size_t file_id )
	{
		lock.lock();
		
		// Add to the file manager to be stored on disk
		size_t fm_id = FileManager::shared()->write( buffer ,  fileName , this );
		
		// Create an item to process callback
		LoadDataManagerItem *i = new LoadDataManagerItem( fileName , fromIdentifier , process_id , file_id , buffer->getSize() );
		item.insertInMap( fm_id , i );
		
		lock.unlock();
	}

	void LoadDataManager::diskManagerNotifyFinish(size_t fm_id, bool success)
	{
		
		LoadDataManagerItem*i =	item.extractFromMap( fm_id );
		
		if( i )
		{
			// Sen a packet bak to delilah to confirm this update
			Packet *p = new Packet();
			network::LoadDataResponse *response = p->message.mutable_load_data_response();
			response->set_process_id( i->process_id );
			response->set_file_id( i->file_id );
			response->set_error( !success );
			
			// Information about the created file
			network::File *file = response->mutable_file();
			file->set_name( i->fileName );
			file->set_worker( worker->myWorkerId );
			network::KVInfo *info = file->mutable_info();
			info->set_kvs(1);
			info->set_size(i->size);
			
			worker->network->send( worker , i->fromIdentifier , Message::LoadDataResponse , p);
		}
	}
	
}
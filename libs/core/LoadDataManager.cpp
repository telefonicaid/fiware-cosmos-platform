#include "LoadDataManager.h"		// Own interface
#include "Buffer.h"					// ss::Buffer
#include "FileManager.h"			// ss::FileManager
#include "SamsonWorker.h"			// ss::SamsonWorker
#include "Packet.h"					// ss::Packet

namespace ss
{

	void LoadDataManager::addFile( Buffer * buffer , std::string fileName, int fromIdentifier )
	{
		lock.lock();
		
		// Add to the file manager to be stored on disk
		size_t id = FileManager::shared()->write( buffer ,  fileName , this );
		
		// Create an item to process callback
		LoadDataManagerItem *i = new LoadDataManagerItem( fileName , fromIdentifier);
		item.insertInMap( id , i );
		
		lock.unlock();
	}

	void LoadDataManager::diskManagerNotifyFinish(size_t id, bool success)
	{
		LoadDataManagerItem*i =	item.extractFromMap( id );
		
		if( i )
		{
			// Sen a packet bak to delilah to confirm this update
			Packet p;
			worker->network->send( worker , i->fromIdentifier , Message::LoadDataResponse , &p);
		}
	}
	
}
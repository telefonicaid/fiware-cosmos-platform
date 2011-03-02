#include "DiskManagerDelegate.h"		// DiskManagerDelegate
#include "DiskOperation.h"				// DiskOperation
#include "DeviceDiskAccessManager.h"	// DeviceDiskAccessManager
#include "DiskManager.h"				// Own interface
#include "au_map.h"						// findInMap(.)


namespace ss {

	static DiskManager* sharedDiskManager=NULL;
	
	DiskManager* DiskManager::shared()
	{
		if( !  sharedDiskManager )
			sharedDiskManager = new DiskManager();
			return sharedDiskManager;
	}
	
	DiskManager::DiskManager()
	{
		counter_id = 0;

	}

	size_t DiskManager::read( char *data , std::string fileName , size_t offset , size_t size , DiskManagerDelegate *delegate )
	{
		size_t id = 0;
		
		lock.lock();
		
		DiskOperation *o = new DiskOperation(counter_id++);
		
		o->fileName = fileName;
		o->mode = "r";
		o->read_buffer = data;
		o->size = size;
		o->offset = offset;
		o->delegate = delegate;
		
		
		if( o->setDevice() )
		{
			_addOperation( o );
			id = o->idGet();
		}
		else
		{
			delete o;
			id = 0;
		}
		
		lock.unlock();
		
		return id;
	}
	
	size_t DiskManager::write( Buffer* buffer ,  std::string fileName , DiskManagerDelegate *delegate )
	{
		size_t id;
		
		lock.lock();
		
		DiskOperation *o = new DiskOperation(counter_id++);
		
		o->fileName = fileName;
		o->mode = "w";
		o->buffer = buffer;
		o->size = buffer->getSize();
		o->offset = 0;
		o->delegate = delegate;
		

		if( o->setDevice() )
		{
			_addOperation( o );
			id = o->idGet();
		}
		else
		{
			delete o;
			id = 0;
		}

		lock.unlock();

		return id;
	}
		
	size_t DiskManager::remove( std::string fileName , DiskManagerDelegate *delegate)
	{
		size_t id;
		
		lock.lock();
		
		DiskOperation *o = new DiskOperation(counter_id++);
		
		o->fileName = fileName;
		o->mode = "x";
		o->delegate = delegate;
		
		if( o->setDevice() )
		{
			_addOperation( o );
			id = o->idGet();
		}
		else
		{
			delete o;
			id = 0;
		}
		
		lock.unlock();
		
		return id;
	}
	
	
	DeviceDiskAccessManager *DiskManager::_getDeviceDiskAccessManagerForDev( dev_t st_dev )
	{
		DeviceDiskAccessManager *device = item.findInMap( st_dev );
		if( device )
			return device;
		else
		{
			// Create a new one, add the the map and return
			DeviceDiskAccessManager *tmp = new DeviceDiskAccessManager();
			item.insertInMap( st_dev, tmp );
			return tmp;
		}
	}
		
	void DiskManager::_addOperation( DiskOperation *o )
	{
		
		DeviceDiskAccessManager *d = _getDeviceDiskAccessManagerForDev( o->st_dev );
		assert( d );
		d->addOperation(o);
		
	}
	
	
	void DiskManager::fill(network::WorkerStatus*  ws)
	{
		lock.lock();
		
		std::ostringstream output;
		au::map <dev_t , DeviceDiskAccessManager>::iterator iter;
		for ( iter = item.begin() ; iter != item.end() ; iter++)
		output << "<<" << iter->second->getStatus() << ">>";
		ws->set_disk_manager_status( output.str() );
		
		lock.unlock();
	}
	
	
	
}

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

		// Setup run-time status report
		setStatusTile( "Disk Manager" , "dm" );
	}

	size_t DiskManager::read( char *data , std::string fileName , size_t offset , size_t size , DiskManagerDelegate *delegate )
	{
		
		lock.lock();
		DiskOperation *o = new DiskOperation(counter_id++);
		
		o->fileName = fileName;
		o->mode = "r";
		o->read_buffer = data;
		o->size = size;
		o->offset = offset;
		o->delegate = delegate;
		
		if( !addOperation( o ) )
			o=NULL;
		lock.unlock();
		
		if( o )
			return o->idGet();
		else
			return 0;
	}
	
	size_t DiskManager::write( Buffer* buffer ,  std::string fileName , DiskManagerDelegate *delegate )
	{
		lock.lock();
		
		DiskOperation *o = new DiskOperation(counter_id++);
		
		o->fileName = fileName;
		o->mode = "w";
		o->buffer = buffer;
		o->size = buffer->getSize();
		o->offset = 0;
		o->delegate = delegate;
		
		if( !addOperation( o ) )
			o=NULL;
		lock.unlock();
		
		if( o )
			return o->idGet();
		else
			return 0;
		
	}
		
	DeviceDiskAccessManager *DiskManager::getDeviceDiskAccessManagerForDev( dev_t st_dev )
	{
		std::map <dev_t , DeviceDiskAccessManager*>::iterator i;
		DeviceDiskAccessManager *device = item.findInMap( st_dev);
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
		
	bool DiskManager::addOperation( DiskOperation *o )
	{
		
		if( !o->setDevice() )
		{
			delete  o;
			return false;
		}
		
		DeviceDiskAccessManager *d = getDeviceDiskAccessManagerForDev( o->st_dev );
		assert( d );
		d->addOperation(o);
		
		return true;
		
	}

	
	void DiskManager::getStatus( std::ostream &output , std::string prefix_per_line )
	{
		output << "\n";
		getStatusFromMap(output, item, prefix_per_line);
	}
	
	
}

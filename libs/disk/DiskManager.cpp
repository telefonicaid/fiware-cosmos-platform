
#include "DiskManager.h"		// Own interface


namespace ss {

	static DiskManager* sharedDiskManager=NULL;

	
	DiskManager* DiskManager::shared()
	{
		if( !  sharedDiskManager )
			sharedDiskManager = new DiskManager();
			return sharedDiskManager;
	}

	void* runDiskAccessItemThread(void * p)
	{
		((DiskAccessItem*)p)->runThread();
		return NULL;
	}
	
	
	
}
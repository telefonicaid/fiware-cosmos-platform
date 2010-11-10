#ifndef _H_LOAD_DATA_MANAGER
#define _H_LOAD_DATA_MANAGER

#include <string>
#include "au_map.h"						// au::map
#include "Lock.h"						// au::Lock
#include "DiskManagerDelegate.h"		// ss::DiskManagerDelegate

namespace ss {

	class Buffer;
	class SamsonWorker;
	
	
	class LoadDataManagerItem
	{

	public:
		
		std::string fileName;
		int fromIdentifier;
		
		LoadDataManagerItem( std::string _fileName , int _fromIdentifier )
		{
			fileName = _fileName;
			fromIdentifier = _fromIdentifier;
		}
	};	
	
	class LoadDataManager : public DiskManagerDelegate
	{
		SamsonWorker *worker;

		au::Lock lock;
		
		au::map<size_t,LoadDataManagerItem> item;
		
	public:
		
		LoadDataManager( SamsonWorker *_worker )
		{
			worker = _worker;
		}
		
		void addFile( Buffer * buffer , std::string fileName, int fromIdentifier );
		
		
		// FileManagerDelegate
		virtual void diskManagerNotifyFinish(size_t id, bool success);

		
	};

}

#endif
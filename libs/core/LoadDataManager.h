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
		size_t process_id;
		size_t file_id;
		size_t size;
		
		LoadDataManagerItem( std::string _fileName , int _fromIdentifier , size_t _process_id , size_t _file_id , size_t _size )
		{
			fileName = _fileName;
			fromIdentifier = _fromIdentifier;
			process_id = _process_id;
			file_id = _file_id;
			size = _size;
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
		
		void addFile( Buffer * buffer , std::string fileName, int fromIdentifier , size_t process_id , size_t file_id );
		
		
		// FileManagerDelegate
		virtual void diskManagerNotifyFinish(size_t id, bool success);

		
	};

}

#endif
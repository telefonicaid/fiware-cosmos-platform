

#include "FileManagerItem.h"			// Own interface

namespace ss {

	FileManagerItem::FileManagerItem( FileManagerDelegate* _delegate ,  Type _type , size_t _size )
	{
		delegate = _delegate;
		type = _type;
		
		// No error by default
		error = false;	
		
		// Type of disk operation for statistics
		switch (type) {
			case read: operation = DiskStatistics::read; break;
			case write: operation = DiskStatistics::write; break;
			case remove: operation = DiskStatistics::unknown; break;
		}
		
		size = _size;
		gettimeofday(&begin, NULL);
	}

	size_t FileManagerItem::getId()
	{
		return fm_id;
	}



	void FileManagerItem::addStatistics(  DiskStatistics * statistics )
	{
		gettimeofday(&end, NULL);
		statistics->add( operation, size , DiskStatistics::timevaldiff( &begin ,&end ) );
	}


	void FileManagerItem::setId(size_t _id)
	{
		fm_id = _id;		// Set the file manager id
	}
	
	void FileManagerItem::setError( std::string _error_message )
	{
		error = true;
		error_message = _error_message;
	}
	
	
}

#include "FileManagerCacheItem.h"		// Own interface


namespace ss {


	FileManagerCacheItem::FileManagerCacheItem(std::string _fileName , Buffer *_buffer)
	{
		fileName = _fileName;	// File name
		buffer = _buffer;		// Buffer where the file is contained
		used_counter = 1;		// Locked until file have been finally saved to disk
	}

	bool FileManagerCacheItem::isUsed()
	{
		return (used_counter > 0);
	}

	void FileManagerCacheItem::destroy()
	{
		MemoryManager::shared()->destroyBuffer( buffer );	
	}

	void FileManagerCacheItem::retain()
	{
		used_counter++;
	}
	
	void FileManagerCacheItem::release()
	{
		used_counter--;
	}

}



/* ****************************************************************************
 *
 * FILE                     FileManagerReadItem.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

#ifndef _H_FILE_MANAGER_READ_ITEM
#define _H_FILE_MANAGER_READ_ITEM


#include "Lock.h"					// au::Locod
#include "Buffer.h"					// ss::Buffer
#include "au_map.h"					// au::map
#include <list>						// std::list
#include <set>						// std::set
#include <vector>					// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "FileManagerItem.h"		// ss::FileManagerItem

namespace ss {
	
	
	class FileManagerReadItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;				// Name of the file
		size_t offset;						// Offset inside file ( only used when reading )
		size_t size;						// Size to read
		
		SimpleBuffer simpleBuffer;			// Simple buffer to write in...
		Buffer *buffer;                                 // Buffer used if provided. ( Note that simpleBuffer is the main element to write in)

		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  SimpleBuffer simpleBuffer , FileManagerDelegate *_delegate );

		// Auxiliar construtor for cases where a buffer is provided instead of a SimpleBuffer
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  Buffer * _buffer , FileManagerDelegate *_delegate );

		// Replace the delegate ( before sending to File Manager )
		void setDelegate( FileManagerDelegate *_delegate );
		
		void notifyToDelegate()
		{
			if( delegate )
				delegate->notifyFinishReadItem(this);
			else
				delete this;	// Auto-remove to myself since no-one will handle me
		}
		
		void freeResources()
		{
			// Nothing to do here
		}
		
		
	};	
}


#endif

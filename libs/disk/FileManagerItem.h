
/* ****************************************************************************
 *
 * FILE                     FileManagerItem.h
 *
 * AUTHOR                   Andreu Urruela
 *
 * CREATION DATE            2010
 *
 */

/*
 
 Notes: 
 
 FileManagerItem is the base class for all the operations that can be squeduled at the FileManger
 
*/


#ifndef _H_FILE_MANAGER_ITEM
#define _H_FILE_MANAGER_ITEM


#include "Lock.h"		// au::Locod
#include "Buffer.h"		// ss::Buffer
#include "au_map.h"		// au::map
#include <list>			// std::list
#include <set>			// std::set
#include <vector>		// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "FileManagerDelegate.h"	// ss::FileManagerDelegate
#include "Error.h"					// au::Error

namespace ss {
	
	/**
	 Generic read/write item that contains a pointer to the delegate
	 */
	
	class FileManagerItem 
	{
	protected:
		
		FileManagerDelegate *delegate;		// Delegate that should be notified
		
		size_t fm_id;						// File Manager id
		
		friend class FileManager;
		
		//For statistics
		
		DiskStatistics::Operation operation;
		
		size_t size;
		
		timeval begin;
		timeval end;

	public:
		
		int component;
		size_t tag;
		size_t sub_tag;
		
	public:
		
		au::Error error;
		
	public:
		
		typedef enum
		{
			write,
			read,
			remove
		} Type;
		
		Type type;
		
		FileManagerItem( FileManagerDelegate* _delegate ,  Type _type , size_t _size );
		
		size_t getId();
		
		void addStatistics(  DiskStatistics * statistics );
		
		
		virtual void notifyToDelegate()=0;
				
		virtual void freeResources()=0;
		
	private:
		void setId(size_t _id);
		
		
	};	
	
	

	
}


#endif


#ifndef _H_FILE_MANAGER
#define _H_FILE_MANAGER


#include "Lock.h"		// au::Locod
#include "Buffer.h"		// ss::Buffer
#include "au_map.h"		// au::map
#include <list>			// std::list
#include <set>			// std::set
#include <vector>		// std::vector
#include "MemoryManager.h"			// ss::MemoryManager
#include "DiskManager.h"			// ss::DiskManager
#include "DiskManagerDelegate.h"	// ss::DiskManagerDelegate

namespace ss {

	/**
	 Generic read/write item that contains a pointer to the delegate
	 */
	
	class FileManagerItem
	{
		FileManagerDelegate *delegate;		// Delegate that should be notified
		
		friend class FileManager;
		
	public:
		FileManagerItem( FileManagerDelegate* _delegate )
		{
			delegate = _delegate;
		}
		
		
		virtual void destroy()
		{
		}
		
		
		virtual std::string getStatus()
		{
			return "";
		}

		
	};
	
	
	class FileManagerReadItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;				// Name of the file
		size_t offset;						// Offset inside file ( only used when reading )
		size_t size;						// Size to read
		
		char *buffer;						// Pointer to where data should be written

		
		static size_t sizeOfFile( std::string fileName )
		{
			struct ::stat info;
			if( stat(fileName.c_str(), &info) == 0)
				return info.st_size;
			else
			{
				assert( false );
				return 0;
			}
		}
		
		
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  char *_buffer , FileManagerDelegate *_delegate ): FileManagerItem( _delegate )
		{
			buffer = _buffer;
			
			fileName = _fileName;
			offset = _offset;
			size = _size;
		}
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "Read Item from file " << fileName << " Offset: " << offset << " Size: " << size;
			return output.str();
		}
		
		
	};
	
	
	class FileManagerWriteItem : public FileManagerItem
	{
		std::string fileName;			// FileName of the file
		Buffer *buffer;					// Buffer with onMemory information

		DiskManagerDelegate *delegate;	// Delegate to notify that this file has been saved

		friend class FileManager;
		
	public:
		
		FileManagerWriteItem( std::string _fileName , Buffer *_buffer , FileManagerDelegate *_delegate ) : FileManagerItem( _delegate )
		{
			fileName = _fileName;
			buffer = _buffer;
		}
		
		
		void destroy()
		{
			if( buffer )
				MemoryManager::shared()->destroyBuffer( buffer );
		}
		
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "Write Item to file " << fileName << " Size: " << buffer->getSize();
			return output.str();
		}
		
		
	};
	
	/**
	 Class used as interface between samson and DiskManager
	 It is used to send files to be saved on disk and to read part of files
	 */
	 
	class FileManager : public DiskManagerDelegate
	{
		au::Lock lock;		// thread safe lock

		size_t current_fm_id;	// Counter to give new file manager ids
		
		// Elements to save to disk
		au::map<size_t, FileManagerItem> items;
		
		// Connections between DiskManager ids and FileManager ids
		au::simple_map<size_t , size_t> ids;

		FileManager()
		{
			// Init the counter to give new ids
			current_fm_id = 0;
		}
		
		
	public:
		
		/** 
		 Singleton implementation
		 */
		
		static FileManager *shared();
		
		size_t addItemToRead( FileManagerReadItem* v )
		{
			lock.lock();
			
			
			size_t fm_id = current_fm_id++;

			// add the the internal map
			items.insertInMap( fm_id , v);

			size_t dm_id = DiskManager::shared()->read( v->buffer , v->fileName , v->offset , v->size,  this );
			
			// add the relation between both ids
			ids.insertInMap( dm_id , fm_id );
			
			lock.unlock();
			
			return fm_id;
			
		};

		size_t addItemToWrite( FileManagerWriteItem* v )
		{
			lock.lock();
			
			size_t fm_id = current_fm_id++;
			
			items.insertInMap( fm_id , v );
			
			size_t dm_id = DiskManager::shared()->write( v->buffer , v->fileName , this );
			
			// add the relation between both ids
			ids.insertInMap( dm_id , fm_id );
			
			lock.unlock();
			
			return fm_id;
		};
		
		
		
		/**
		 Delegate interface of DiskManager
		 It is notified when a write / read is finished
		 */
		
		void diskManagerNotifyFinish(size_t id, bool success)
		{
			FileManagerDelegate *delegate = NULL;
			
			lock.lock();
			
			size_t dm_id = id;
			
			if( ids.isInMap( dm_id ) )
			{
			   
				size_t fm_id = ids.extractFromMap( dm_id );
				
				FileManagerItem *item = items.extractFromMap( fm_id );
				
				if( item )
				{
					item->destroy();	// What ever you want to do with it
					
					delegate = item->delegate;
					delete item;					// we are responsible for deleting this 
				}
				
			}
			else
			{
				assert(false);// No idea what to do...
			}
			
			lock.unlock();
			
			// Call the delegate outside the lock to avoid dead-lock
			if( delegate )
				delegate->fileManagerNotifyFinish( id , success); 
		}


		
		/**
		 File manager status string
		 */
		
		std::string getStatus()
		{
			std::ostringstream output;

			std::map<size_t, FileManagerItem*>::iterator iter;
			for (iter = items.begin() ; iter!= items.end() ;iter++)
				output << iter->second->getStatus() << std::endl;
			return output.str();
		}

		
		
	};


}
#endif
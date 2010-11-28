
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
	 Notes:

	 Writing:
	 DataBuffer sends files to be saved on disk
	 When finish, DataBuffer is notified about this
	 Internal files can be keept in memory to see if we receive read commands for the same file ( very common in concatenated operations )
	 DataBuffer cannot assume the buffer sent to save is on memory anymore
	 Buffers are destroyed by FileManager when necessary
	 
	 Reading:
	 Process may add an items to read from disk.
	 They can check if all of them are ready and lock them so FileManager do not remove from memory
	 You never know if DataBuffer is able to requeue content of a file from a previous save or it loads content from disk
	 FileManager allocate the buffer for you
	 Process are responsible to remove these items from FileManager
	 
	 */
	
	class FileManagerReadItem
	{
		
	public:
		
		std::string fileName;		// Name of the file
		size_t offset;		// Offset inside file ( only used when reading )
		size_t size;		// Size to read

		FileManagerReadItem( std::string _fileName )
		{
			fileName = _fileName;

			offset = 0;
			// get the size of this file
			struct ::stat info;
			if( stat(fileName.c_str(), &info) == 0)
			{
				size = info.st_size;
			}
			else
			{
				assert( false );
				size = 0;
			}
		}
		
		
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size )
		{
			fileName = _fileName;
			offset = _offset;
			size = _size;
		}
	};


	/**
	 Vector of items to be read from disk
	 This is the structure submited by any process
	 This should never be deleted outsite FileManager (only report that finish to be used )
	 */
	
	class FileManagerReadItemVector
	{
	public:
		
		char *data;
		size_t max_size;

		size_t size;	// acumulated size to read
		
		std::vector<FileManagerReadItem> items;

		int read_items;
		
		FileManagerReadItemVector(  )
		{
			size = 0;
			read_items = 0;
		}
		
		void setBuffer( char *_data , size_t _max_size )
		{
			assert( _max_size >= size );
			data = _data;
			max_size = _max_size;
		}
		
		void addItem( FileManagerReadItem item )
		{
			items.push_back( item );

			// Keep the total size for debuggin
			size+= item.size;

		}
		
		
		void notifyReadFile()
		{
			read_items++;
		}
		
		bool isReady()
		{
			return (read_items == (int)items.size());
		}
		
		/** 
		 Remove all the buffers inside
		 */
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << "Vector with " << items.size() << " files";
			if( isReady() )
				output << " READY ";
			else 				
				output << " NOT READY ";

			return output.str();
		}
			
		
	};
	
	
	/**
	 Class used to store a file that has to be saved to disk
	 */
	
	class FileManagerWriteItem 
	{
		std::string fileName;			// FileName of the file
		Buffer *buffer;					// Buffer with onMemory information
		bool ready;						// File has been saved so, it can be removed from memory if necessary

		DiskManagerDelegate *delegate;	// Delegate to notify that this file has been saved

		friend class FileManager;
		
	public:
		
		FileManagerWriteItem( std::string _fileName , Buffer *_buffer , DiskManagerDelegate *_delegate )
		{
			fileName = _fileName;
			buffer = _buffer;
			delegate = _delegate;
			
			ready = false;
		}
	
		
		void setReady()
		{
			ready = true;
		}
		
	};
	
	/**
	 Class used as interface between samson and DiskManager
	 It is used to send files to be saved on disk and to read part of files
	 */
	 
	class FileManager : public DiskManagerDelegate
	{
		au::Lock lock;		// thread safe lock
		
		// Elements to save to disk
		au::map<std::string, FileManagerWriteItem> itemsToSave;
		
		// Elements to read ( in order of preference )
		std::set< FileManagerReadItemVector* > itemsToRead;
		
		// Connections between DiskManager ids and fileNames of file saving
		au::map<size_t , FileManagerWriteItem> savingFiles;
		
		// Connections between DiskManager ids and itemsToRead
		au::map<size_t , FileManagerReadItemVector> readingFiles;
		
	public:
		
		/** 
		 Singleton implementation
		 */
		
		static FileManager *shared();
		
		/**
		 Send a file to be saved
		 DataBuffer is notified when finished using XXX
		 Buffer is automatically destroyed when finish or when memory is scarse
		 */
		
		size_t write( Buffer* buffer ,  std::string fileName , DiskManagerDelegate *delegate )
		{
			lock.lock();
		
			FileManagerWriteItem *tmp = new FileManagerWriteItem( fileName , buffer, delegate );
			
			itemsToSave.insertInMap( fileName , tmp );

			size_t id = DiskManager::shared()->write( buffer , fileName , this );

			savingFiles.insertInMap( id , tmp );
			
			lock.unlock();
			
			return id;
		};
		
		/** 
		 Request a set of files to be read for a particular operation
		 */

		void addItemsToRead( FileManagerReadItemVector* v )
		{
			lock.lock();
			itemsToRead.insert( v );

			
			// Set all read from disk ( in the future, we will use cached elements from memory )

			size_t offset = 0 ;

			for (size_t i = 0 ; i < v->items.size() ; i++)
			{
				size_t id = DiskManager::shared()->read( v->data + offset , v->items[i].fileName , v->items[i].offset , v->items[i].size,  this );
				offset += v->items[i].size;
				
				readingFiles.insertInMap( id , v );
			}
			
			
			
			lock.unlock();
			
		};
		
		/**
		 Delegate interface of DiskManager
		 It is notified when a write / read is finished
		 */
		
		void diskManagerNotifyFinish(size_t id, bool success)
		{
			DiskManagerDelegate *delegate = NULL;
			
			lock.lock();

			// See if it is a save file
			FileManagerWriteItem* item = savingFiles.extractFromMap( id );

			if( item )
			{
				// Set ready flag so it can be removed from memory if necessarry
				item->setReady();
				
				// Notify to my delegate
				delegate = item->delegate;

			}
		
			// See if it is a read file
			FileManagerReadItemVector* v = readingFiles.extractFromMap( id );

			if( v )
			{
				v->notifyReadFile( );
				if( v->isReady() )
				{
					// Notify someone...
					// delegate = ?
					
					// Remove from this list
					itemsToRead.erase( v );
					
					
				}
			}
			
			lock.unlock();
			
			// Call the delegate outside the lock to avoid dead-lock
			if( delegate )
				delegate->diskManagerNotifyFinish( id , success); 
		}

		
		
	};


}
#endif
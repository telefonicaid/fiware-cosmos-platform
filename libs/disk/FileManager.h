
#ifndef _H_FILE_MANAGER
#define _H_FILE_MANAGER


#include "Lock.h"		// au::Locod
#include "Buffer.h"		// ss::Buffer
#include "au_map.h"		// au::map
#include <list>			// std::list
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
		std::string fileName;		// Name of the file
		size_t offset;		// Offset inside file ( only used when reading )
		size_t size;		// Size to read
		
		Buffer *buffer;		// Buffer used to read ( created by MemoryManager ) 
		bool ready;			// flag to indicate that the content is ready (content is there)
		
		friend class FileManagetReadItemVector;
		
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size )
		{
			fileName = _fileName;
			offset = _offset;
			size = _size;
			
			buffer = NULL;
			ready = false;
		}
	};


	/**
	 Vector of items to be read from disk
	 This is the structure submited by any process
	 */
	
	class FileManagetReadItemVector : public std::vector<FileManagerReadItem>
	{
		bool lock;	// Flag to indicate that this content is being used outside MemoryManager so we cannot free from memory any buffer

	public:
		
		FileManagetReadItemVector()
		{
			lock = false;
		}
		
		
		bool checkAndLock()
		{
			assert( false ); // TODO: complete operation
			return false;
		}
		
		/** 
		 Remove all the buffers inside
		 */
		
		void destroy()
		{
			for (size_t i = 0 ; i < size() ; i++)
				MemoryManager::shared()->destroyBuffer( (*this)[i].buffer );
		}
		
	};
	
	
	/**
	 Class used to store a file that has to be saved to disk
	 */
	
	class FileManagerWriteItem
	{
		std::string fileName;		// FileName of the file
		Buffer *buffer;				// Buffer with onMemory information
		bool ready;					// File has been saved so, it can be removed from memory if necessary

	public:
		
		FileManagerWriteItem( std::string _fileName , Buffer *_buffer)
		{
			fileName = _fileName;
			buffer = _buffer;
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
	 
	class FileManager : public DiskManagerDelegate	// Implements delegate of the DiskManager
	{

		au::Lock lock;		// thread safe lock

		
		// Elements to save to disk
		au::map<std::string, FileManagerWriteItem> itemsToSave;
		
		// Elements to read ( in order of preference )
		std::list< FileManagetReadItemVector* > itemsToRead;
		
		
		// Connections between DiskManager ids and fileNames of file saving
		au::map<size_t , FileManagerWriteItem> savingFiles;
		
	public:
		
		/**
		 Send a file to be saved
		 DataBuffer is notified when finished using XXX
		 Buffer is automatically destroyed when finish or when memory is scarse
		 */
		
		void saveFile(  std::string fileName , Buffer *buffer )
		{
			lock.lock();
		
			FileManagerWriteItem *tmp = new FileManagerWriteItem( fileName , buffer );
			
			itemsToSave.insertInMap( fileName , tmp );

			size_t id = DiskManager::shared()->write( buffer , fileName , this );

			savingFiles.insertInMap( id , tmp );
			
			lock.unlock();
		};
		
		/** 
		 Request a set of files to be read for a particular operation
		 */

		void addItemsToRead( FileManagetReadItemVector* v )
		{
			lock.lock();
			itemsToRead.push_back( v );
			lock.unlock();
			
		};

		/**
		 Function used to check if all the required files are on memory and lock them if possible
		 */
		 
		bool checkAndLockItemsToRead( FileManagetReadItemVector* v)
		{ 
			bool answer;
			lock.lock();
			answer = v->checkAndLock();
			lock.unlock();
			
			return answer;
		}
		
		/**
		 Function used to remove a list of files to read
		 */
		
		void removeItemsToRead( FileManagetReadItemVector* v )
		{
			// Destroy al buffers used here, and then remove the element from the list
			lock.lock();
			
			std::list< FileManagetReadItemVector* >::iterator i;
			for ( i = itemsToRead.begin(); i != itemsToRead.end() ; i++)
			{
				if( *i == v )
				{
					// Remove from the list
					itemsToRead.erase( i );
					
					// Delete buffers and the element itself
					v->destroy();
					delete v;
					
					lock.unlock();
					return ;
				}
			}
			
			assert( false );	// Something wrong happen
			
		}
		
		
		/**
		 Delegate interface of DiskManager
		 It is notified when a write / read is finished
		 */
		
		void diskManagerNotifyFinish(size_t id, bool success)
		{
			lock.lock();

			// See if it is a save file
			FileManagerWriteItem* item = savingFiles.extractFromMap( id );
			if( item )
				item->setReady();
			
			lock.unlock();
		}

		
		
	};


}
#endif
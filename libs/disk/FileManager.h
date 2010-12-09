
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
#include "DiskStatistics.h"			// ss::DiskStatistics
#include "Status.h"					// au::Status

namespace ss {

	/**
	 Generic read/write item that contains a pointer to the delegate
	 */
	
	class FileManagerItem : public au::Status
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
		
		
		typedef enum
		{
			write,
			read
		} Type;

		Type type;
		
		FileManagerItem( FileManagerDelegate* _delegate ,  Type _type , size_t _size )
		{
			delegate = _delegate;
			type = _type;

			// Type of disk operation for statistics
			switch (type) {
				case read: operation = DiskStatistics::read; break;
				case write: operation = DiskStatistics::write; break;
			}
			
			size = _size;
			gettimeofday(&begin, NULL);
		}
		
		size_t getId()
		{
			return fm_id;
		}
		
		
		
		void addStatistics(  DiskStatistics * statistics )
		{
			gettimeofday(&end, NULL);
			statistics->add( operation, size , DiskStatistics::timevaldiff( &begin ,&end ) );
		}
		
	private:
		void setId(size_t _id)
		{
			fm_id = _id;		// Set the file manager id
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
		
		
		FileManagerReadItem( std::string _fileName , size_t _offset , size_t _size ,  char *_buffer , FileManagerDelegate *_delegate )
			: FileManagerItem( _delegate , FileManagerItem::read , _size )
		{
			buffer = _buffer;
			
			fileName = _fileName;
			offset = _offset;
			size = _size;
		}
		
		
		// Replace the delegate ( before sending to File Manager )
		void setDelegate( FileManagerDelegate *_delegate )
		{
			delegate = _delegate;
		}
		
		void getStatus( std::ostream &output , std::string prefix_per_line )
		{
			output << "Read Item File: " << fileName << " Offset: " << offset << " Size: " << au::Format::string( size , "bytes") << "\n";
		}
		
	};
	
	
	class FileManagerWriteItem : public FileManagerItem
	{
		
	public:
		
		std::string fileName;			// FileName of the file
		Buffer *buffer;					// Buffer with onMemory information

		DiskManagerDelegate *delegate;	// Delegate to notify that this file has been saved

		FileManagerWriteItem( std::string _fileName , Buffer *_buffer , FileManagerDelegate *_delegate ) 
		: FileManagerItem( _delegate , FileManagerItem::write , _buffer->getSize() )
		{
			fileName = _fileName;
			buffer = _buffer;
		}
		
		void getStatus( std::ostream &output , std::string prefix_per_line )
		{
			output << "Write Item File: " << fileName << " Size: " << au::Format::string( buffer->getSize() , "bytes" ) << "\n";
		}
		
		
		
	};
	
	
	
	class FileManagerCacheItem
	{
		
	public:
		
		std::string fileName;
		Buffer *buffer;
		int used_counter;				// Number of internal operations that lock this element 
										// It can be removed from memory is used_counter == 0
		
		FileManagerCacheItem(std::string _fileName , Buffer *_buffer)
		{
			fileName = _fileName;	// File name
			buffer = _buffer;		// Buffer where the file is contained
			used_counter = 0;		// No used by default
		}
		
		bool isUsed()
		{
			return (used_counter > 0);
		}
		
		void destroy()
		{
			MemoryManager::shared()->destroyBuffer( buffer );	
		}
		
		void retain()
		{
			used_counter++;
		}
		void release()
		{
			used_counter--;
		}
		
	};
	
	
	class FileManagerCacheTask
	{
		
	public:
		
		FileManagerReadItem *readItem;		// Item to be read
		FileManagerCacheItem *cacheItem;	// Item to read from
		
		FileManagerCacheTask(FileManagerReadItem *_readItem , FileManagerCacheItem *_cacheItem)
		{
			readItem = _readItem;
			cacheItem = _cacheItem;
		}
		
	};
	
	void* runFileManagerCacheSystemThread(void*);
	
	class FileManagerCacheSystem
	{
		
		// Cache of files
		size_t size_in_cache;										// Total memory used by cache
		au::ListMap <std::string , FileManagerCacheItem> cache;		// Double-linked list of cache elements

		// List of pending tasks
		std::list<FileManagerCacheTask*> tasks;
		
		pthread_t t;	// Main thread of file manager cache system
		
		au::Lock lock;
		au::StopLock stopLock;
		
	public:
		
		FileManagerCacheSystem() : stopLock( &lock )
		{
			// Reset size 
			size_in_cache = 0;
			
			pthread_create(&t, NULL, runFileManagerCacheSystemThread, this);
		}
		
		void run();
		
		bool freeMemoryIfPossible()
		{
			bool answer = false;
			lock.lock();
			
			FileManagerCacheItem *item = cache.extractFromBack();
			
			if( item && !item->isUsed() )
			{
				size_in_cache -= item->buffer->getSize();
				
				item->destroy();
				answer = true;
				delete item;
			}
			
			lock.unlock();
			return answer;
		}
		
		/**
		 Add a request for a file
		 Return true, if it is in the cache and can be served
		 */
		
		bool addReadItem( FileManagerReadItem *read_item )
		{
			bool answer = false;
			
			lock.lock();
			
			FileManagerCacheItem *item = cache.extractFromMap( read_item->fileName );

			if( item )
			{
				answer = true;		// Yes, it can be served from cache
				item->retain();
				
				// Reorder
				cache.insertAtFront( read_item->fileName , item);
				
				// add to the list of pending tasks
				FileManagerCacheTask *task = new FileManagerCacheTask( read_item , item );
				tasks.push_back( task );
			}
			
			lock.unlock();
			
			lock.wakeUpStopLock( &stopLock );
			
			return answer;
		}
		
		/** 
		 Add a buffer to the cache
		 */
		
		void addWriteItem( FileManagerWriteItem *write_item)
		{
			// Add an element to the cache
			lock.lock();
			
			FileManagerCacheItem *item = cache.extractFromMap( write_item->fileName );
			if( item )
			{
				// Strange case... the best is to destroy incomming buffer and forget about it
				MemoryManager::shared()->destroyBuffer( write_item->buffer );
			}
			else
			{
				FileManagerCacheItem *new_item = new FileManagerCacheItem( write_item->fileName , write_item->buffer );
				cache.insertAtFront( write_item->fileName, new_item);
				
				size_in_cache += new_item->buffer->getSize();

			}
			lock.unlock();
		}
		
		std::string getStatus()
		{
			std::ostringstream output;
			output << cache.size() << " items with " << au::Format::string( size_in_cache , "bytes" ); 
			return output.str();
		}
		
	};
	
	
	/**
	 Class used as interface between samson and DiskManager
	 It is used to send files to be saved on disk and to read part of files
	 */
	 
	class FileManager : public DiskManagerDelegate , public au::Status
	{
		// thread safe lock
		au::Lock lock;			

		// Counter to give new file manager ids
		size_t current_fm_id;	
		
		// Elements to save or read
		au::map<size_t, FileManagerItem> items;
		
		// Connections between DiskManager ids and FileManager ids
		au::simple_map<size_t , size_t> ids;

		// Cache system for file manager
		FileManagerCacheSystem cacheSystem;
		
		//Statistics
		DiskStatistics statistics;
		
		FileManager()
		{
			// Init the counter to give new ids
			current_fm_id = 0;
			setStatusTile( "File Manager" , "fm" );

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
			v->setId(fm_id);
			
			// add the the internal map
			items.insertInMap( fm_id , v);

			if( ! cacheSystem.addReadItem( v ) )
			{

				// Add to the disk manager
				size_t dm_id = DiskManager::shared()->read( v->buffer , v->fileName , v->offset , v->size,  this );
			
				// add the relation between both ids
				ids.insertInMap( dm_id , fm_id );
			}
			
			lock.unlock();
			
			return fm_id;
			
		};

		size_t addItemToWrite( FileManagerWriteItem* v )
		{
			lock.lock();
			
			size_t fm_id = current_fm_id++;
			v->setId(fm_id);
			
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
		
		void diskManagerNotifyFinish( size_t id, bool success)
		{
			
			lock.lock();
			
			size_t dm_id = id;
			
			if( ids.isInMap( dm_id ) )
			{
				size_t fm_id = ids.extractFromMap( dm_id );
				lock.unlock();
				
				finishItem( fm_id , success );
				return;
				
			}
			else
			{
				lock.unlock();
				assert(false);// No idea what to do...
			}
			
			
		}

		
		void finishItem( size_t fm_id , bool success )
		{
			FileManagerDelegate *delegate = NULL;
			
			lock.lock();
			
			FileManagerItem *item = items.extractFromMap( fm_id );
			
			if( item )
			{
				
				item->addStatistics( &statistics );

				if( item->type == FileManagerItem::write )
					cacheSystem.addWriteItem( (FileManagerWriteItem*) item );
				
				delegate = item->delegate;
				delete item;							// we are responsible for deleting this 
			}
			
			lock.unlock();
			
			// Call the delegate outside the lock to avoid dead-lock
			assert(delegate);	// It is not allowed non-delegate calls
			
			if( delegate )
				delegate->fileManagerNotifyFinish( fm_id , success); 
			
		}
		
	public:
		
		// Function to get the run-time status of this object
		void getStatus( std::ostream &output , std::string prefix_per_line );

		
		
	};


}
#endif
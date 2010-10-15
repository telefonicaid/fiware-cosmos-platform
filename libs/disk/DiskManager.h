
#ifndef _H_DISK_MANAGER
#define _H_DISK_MANAGER

#include <stdio.h>			// fopen , fclose, ...
#include <string>			// std::string
#include <map>				// std::map
#include <set>				// std::set
#include <list>				// std::list
#include "Lock.h"			// au::Lock
#include "StopLock.h"		// au::StopLock
#include <pthread.h>        /* pthread_mutex_init, ...                  */
#include <sys/stat.h>		// stat(.)
#include <iostream>			// std::cout
#include <sstream>			// std::ostringstream
#include <Format.h>			// au::Format
 #include <time.h>			// clock(.)

namespace ss {

	
	class FileAccess
	{
		
		std::string _fileName;	// A copy of the fileName
		FILE *_file;			// File pointer if it is still open
		std::string _mode;		// Mode of the fopen
		
		
		void init( )
		{
			if( _file )
				fclose(_file);
			_file = fopen(_fileName.c_str(), _mode.c_str());
		}
		
		void close()
		{
			if( _file) 
			{
				fclose(_file);
				_file = NULL;
			}
		}
		
	public:
		
		FileAccess( std::string fileName , std::string mode )
		{
			_fileName = fileName;
			_mode = mode;
			
			_file = NULL;
			init();
		}
		
		~FileAccess()
		{
			close();
		}

		void setMode( std::string mode )
		{
			_mode = mode;
			close();
			init();
		}

		bool read( char *buffer , size_t offset , size_t size)
		{
			if( _mode != "r" )
				return false;
			
			// Seek to the rigth offset
			fseek(_file, offset, SEEK_SET);
			
			// read to the buffer
			if( fread(buffer, 1, size, _file) == 1 )
				return true;
			else
				return false;
		}
		
		
		bool append( char *buffer , size_t size)
		{
			
			assert( _mode == "w" );
			assert( _file );
			
			// read to the buffer
			if( fwrite(buffer, 1, size, _file) == 1 )
			{
				fflush(_file);
				return true;
			}
			else
				return false;
		}
		
		
		
	};
	
	class DiskManagerDelegate
	{
	public:
		// Notify that this operation has finished
		virtual void diskManagerNotifyFinish( size_t id , bool success )=0;	
	};
	
	
	class DiskOperation
	{
		size_t _id;

	public:
		
		DiskOperation( size_t id )
		{
			_id = id;
		}
		
		std::string fileName;			// FileName to open
		std::string mode;				// Mode to open
		char *buffer;					// Buffer to store / read data
		size_t size;					// Size to read/write
		size_t offset;					// Offset inside the file ( onlu for read operations )
		dev_t st_dev;					// Device where this file is stored
			
		DiskManagerDelegate *delegate;	// Who should be notified
		
		size_t idGet()
		{
			return _id;
		}
		
		static std::string directoryPath( std::string path )
		{
			size_t pos = path.find_last_of("/");
			
			if ( pos == std::string::npos)
				return ".";
			else
				return path.substr( 0 , pos );
		}
		
		bool setDevice( )
		{
			
			struct ::stat info;
			stat(fileName.c_str(), &info);
			
			if( S_ISREG(info.st_mode) )
			{
				st_dev = info.st_dev;	// Get the devide of the inode 
				return true;
			}
			
			// File currently does not exist ( it wil be writed )
			if( mode != "w" )
				return false;
			
			// Get the path of the directory
			std::string directory = directoryPath( fileName );
			stat(directory.c_str(), &info);
			
			if( S_ISDIR(info.st_mode) )
			{
				st_dev = info.st_dev;	// Get the devide of the inode 
				return true;
			}
			

			// Something wrong...
			return false;
		}
	
	};
	
	
	void* runDiskAccessItemThread(void * p);
	
#define DiskAccessItem_max_open_files	100
	
	class DiskAccessItem
	{
		au::Lock lock;
		au::StopLock stopLock;
		std::list<DiskOperation*> operation;			// List of pending operations

		// Internal file map ( only modifies in the run call ( so it is not protected by lock)
		std::map <std::string , FileAccess*> files;		// Information about open files
		
		pthread_t t;
		bool finished;
		
		size_t total_size;	// Total size 
		double total_time;	// total seconds
		
	public:
		
		DiskAccessItem() : stopLock( &lock )
		{
			// Finish variable to controll threads
			finished = false;	
			
			total_size = 0;
			total_time = 0;
			
			// Create the thread for this disk operations
			pthread_create(&t, NULL, runDiskAccessItemThread, this);
		}

		DiskOperation * getNextOperation()
		{
			DiskOperation *o = NULL;

			lock.lock();
			if( operation.size() > 0)
			{
				o = operation.front();
				operation.pop_front();
			}
			lock.unlock();
			return o;
		}
		
		void runThread()
		{
			while( !finished )
			{
				DiskOperation *o = getNextOperation();
					
				if( o)
					run (o);
				else
				{
					std::cout << "Thread disk block" << std::endl;
					 
					lock.lock();
					lock.unlock_waiting_in_stopLock( &stopLock );
					
					std::cout << "Thread disk wake up" << std::endl;
					
				}
			}
		}
		
		void addOperation(DiskOperation* o)
		{
			
			lock.lock();
			operation.push_back( o );	
			lock.unlock();
			
			// Wake up the thread if necessary
			lock.wakeUpStopLock( &stopLock );	
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "Disk";
			o << " Files: " << files.size();
			o << " Ops: " << operation.size();
			o << " Size: " << au::Format::string( total_size );
			o << " Time: " << au::Format::time_string( total_time );
			o << " Rate: " << au::Format::string( (double) total_size / (double) total_time );
			return o.str();
		}
		
	private:
		
		void run( DiskOperation *o)
		{
			// Get the file information
			FileAccess *file = getFile( o->fileName  , o->mode );
			
			time_t start, stop;
			time(&start);
			
			if ( o->mode == "r" )
				file->read(o->buffer, o->offset , o->size);
			else
				file->append(o->buffer, o->size);

			time(&stop);
			
			total_size += o->size;
			total_time += difftime(stop, start);
			
			// Nofity end of a task
			o->delegate->diskManagerNotifyFinish(o->idGet(), true);
			
			
		}
		
		FileAccess* getFile( std::string fileName , std::string mode )
		{

			std::map <std::string , FileAccess*>::iterator f;
			
			f =  files.find( fileName );
			if( f != files.end() )
			{
				f->second->setMode( mode );
				return f->second;
			}
			else
			{
				// We should close a file here...
				
				FileAccess *f = new FileAccess( fileName , mode );
				files.insert( std::pair<std::string , FileAccess*> ( fileName , f ) );
				
				return f;
			}
				
		}
		
		
	};
	
	class DiskManager
	{
		// Global lock for thread safe implementation
		au::Lock lock;
		
		// All the files that are open
		std::map <dev_t , DiskAccessItem*> item;		// All partitions we can access
		size_t counter_id;
		
		
		DiskManager()
		{
			counter_id = 0;
		}
		
	public:

		
		/**
		 Singleton implementation of disk manager
		 */
		static DiskManager* shared();
		
		
		/**
		 Schedule a read operation
		 Return inmediatelly n
		 Latter a notification is send to the delegate
		 */
		size_t read( char *buffer , size_t size ,  std::string fileName , size_t offset , DiskManagerDelegate *delegate )
		{
			
			lock.lock();
			DiskOperation *o = new DiskOperation(counter_id++);
			
			o->fileName = fileName;
			o->mode = "r";
			o->buffer = buffer;
			o->size = size;
			o->offset = offset;
			o->delegate = delegate;

			if( !addOperation( o ) )
				o=NULL;
			lock.unlock();
		
			if( o )
				return o->idGet();
			else
				return 0;
		}
		
		/**
		 Schedule a write operation
		 Return inmediatelly
		 Latter a notification is send to the delegate
		 */
		size_t write( char *buffer , size_t size ,  std::string fileName , DiskManagerDelegate *delegate )
		{
			lock.lock();
			DiskOperation *o = new DiskOperation(counter_id++);
			
			o->fileName = fileName;
			o->mode = "w";
			o->buffer = buffer;
			o->size = size;
			o->offset = 0;
			o->delegate = delegate;
			
			if( !addOperation( o ) )
				o=NULL;
			lock.unlock();
			
			if( o )
				return o->idGet();
			else
				return 0;
			
		}
		
		
		void showStatus()
		{
			lock.lock();
			std::cout << "DiskManager" << std::endl;
			for ( std::map <dev_t , DiskAccessItem*>::iterator i =  item.begin() ; i != item.end() ; i++ )
				std::cout << i->second->str() << std::endl;
				
			
			lock.unlock();
		}
		
	private:
		
		DiskAccessItem *getDiskAccessItemForDev( dev_t st_dev )
		{
			std::map <dev_t , DiskAccessItem*>::iterator i;
			i =  item.find( st_dev );
			if( i != item.end() )
				return i->second;
			else
			{
				// Create a new one, add the the map and return
				DiskAccessItem *tmp = new DiskAccessItem();
				item.insert( std::pair<dev_t , DiskAccessItem*>( st_dev , tmp ) );
				return tmp;
			}
		}

		

		
		bool addOperation( DiskOperation *o )
		{

			if( !o->setDevice() )
			{
				delete  o;
				return false;
			}
			
			
			DiskAccessItem *d = getDiskAccessItemForDev( o->st_dev );
			assert( d );
			d->addOperation(o);
			
			return true;
			
		}
		
		
	};

	
	class DiskOperationsManager : public ss::DiskManagerDelegate
	{
		ss::DiskManager *dm;
		
		std::set<size_t> codes;			// Codes of all pending operations
		std::set<size_t> failedCodes;	// Codes of all pending operations
		
		au::Lock lock;
		
	public:
		
		DiskOperationsManager()
		{	
			// Get the pointer to the shared Disk Manager
			dm = ss::DiskManager::shared();
		}
		
		
		
		virtual void diskManagerNotifyFinish( size_t id , bool success )
		{
			lock.lock();
			if( !success )
				failedCodes.insert( id );
			codes.erase( id );
			lock.unlock();
			
			// Show information about disk access
			dm->showStatus();
			
		}
		
		void read( char *buffer , size_t size ,  std::string fileName , size_t offset )
		{
			std::cout << "Scheduled read on " << fileName << std::endl;
			
			lock.lock();
			size_t id = dm->read( buffer , size , fileName , offset , this );
			if( id )
				codes.insert( id );
			lock.unlock();
			
		}
		
		void write( char *buffer , size_t size ,  std::string fileName  )
		{
			std::cout << "Scheduled to write on " << fileName << std::endl;
			
			lock.lock();
			size_t id = dm->write( buffer , size , fileName , this );
			if( id )
				codes.insert( id );
			lock.unlock();
			
		}
		
		bool finishedAllOperation()
		{
			lock.lock();
			bool f = ( codes.size() == 0 );
			lock.unlock();
			return f;
		}
		
		void waitUntilComplete()
		{
			while (true) {
				if( finishedAllOperation() )
					return;
				sleep(1);
			}
			
		}
		
		
	};	
	
}


#endif
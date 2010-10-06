#include <math.h>
#include <algorithm>

#include "Data.h"
#include "samson.h"
#include "KVQueue.h"
#include "ModulesManager.h"
#include "KvInfo.h"              /* KVInfo                                   */
#include "KVSet.h"               /* Own interface                            */



namespace ss {

	
#pragma mark KVSetData
	
	
	/**
	 Flush contents of the buffer to disk
	 */
	
	void KVSetData::flushToDisk()
	{
		assert(on_memory);
		
		if( bufferSize > 0 )	// Prevent 0-sized KVSets
		{
			FILE*  _file = fopen(file_name.c_str(), "w");
			int    ans;

			if (_file == NULL)
			{
				LOG_ERROR(("error opening '%s' for writing: %s", file_name.c_str(), strerror(errno)));
				return;
			}

			ans = fwrite(buffer, bufferSize, 1, _file);
			if (ans != 1)
			{
				if (ans == -1)
					LOG_ERROR(("fwrite: %s", strerror(errno)));
				else
					LOG_ERROR(("fwrite: written %d bytes instead of %d", ans, 1));
			}

			fclose(_file);
		}
		
		// This is done inside KVManager for better lock mechanishm
		//on_disk = true;
	}
	
	void KVSetData::loadFromDisk()
	{
		
		/**
		 It is assumed that who calls this method should have been asked for memory
		 Memory Controller
		 */
		assert( on_disk );
		assert( !on_memory );
		assert( !buffer );
		
		buffer	= (char *) malloc( bufferSize );
		
		FILE*  _file = fopen(file_name.c_str(), "r");
		int    ans;

		if (_file == NULL)
		{
			LOG_ERROR(("error opening '%s' for reading: %s", file_name.c_str(), strerror(errno)));
			return;
		}

		ans = fread(buffer, bufferSize, 1, _file);
		if (ans != 1)
		{
			if (ans == -1)
				LOG_ERROR(("fwrite: %s", strerror(errno)));
			else
				LOG_ERROR(("fwrite: written %d bytes instead of %d", ans, 1));
		}

		fclose(_file);
		
		// This is done inside KVManager for better lock mechanishm
		//on_memory = true;
	}
	
	size_t KVSetData::freeBuffer( )
	{
		
		if( on_memory ) 
		{
			free( buffer );
			buffer = NULL;
			on_memory = false;
		}
		
		
		return bufferSize;
	}
	
	
#pragma mark KVSETBASE
	
	KVSetBase::KVSetBase()
	{
		num_tasks = 0;		// By default it is not demanded by any tas<k
		useCounter = 0;		// By default it is not used
	}

		
#pragma mark Notifictions
	
	
	void KVSet::addQueue(KVQueue *queue)
	{
		queues.push_back( queue );
		num_queues = queues.size();
		
		// Notify to this queue to be updated
		KVInfoData data( getNumKVs() , getSize() , isOnMemory()?getSize():0 , isOnDisk()?getSize():0 );
		queue->addKVInfoData( &data );
		
		new_set = false;	// Not considered a new KVSet any more. Now it will  be removed if not assigned to any task / queue
		
	}
	
	void KVSet::removeQueue(KVQueue *queue)
	{
		std::vector<KVQueue*>::iterator q = find(queues.begin(), queues.end(), queue);
		
		// Assert to make sure we only remove queues when it was previously added
		assert( q != queues.end() );
		
		queues.erase( q );
		
		num_queues = queues.size();
		
		// Notify to this queue to be updated
		KVInfoData data( getNumKVs() , getSize() , isOnMemory()?getSize():0 , isOnDisk()?getSize():0 );
		queue->removeKVInfoData( &data );
	}
	
	
	void KVSet::notifyFlushKVSetToDisk()
	{
		KVInfoData data( 0 , 0 , 0 , getSize() );
		
		for (size_t i = 0  ;i < queues.size() ; i++)
			queues[i]->addKVInfoData( &data );
	}
	void KVSet::notifyReadKVSetFromDisk()
	{
		KVInfoData data( 0 , 0 , getSize() , 0 );
		for (size_t i = 0  ;i < queues.size() ; i++)
			queues[i]->addKVInfoData( &data );
	}
	void KVSet::notifyFreeKVSet()
	{
		KVInfoData data( 0 , 0 , getSize() , 0 );
		for (size_t i = 0  ;i < queues.size() ; i++)
			queues[i]->removeKVInfoData( &data );
	}
	

	
	
#pragma mark KVSET

	
	KVSet::KVSet( size_t _id , size_t _task_id,  std::string fileName,  KVSetBufferBase * buffer ) : KVSetData( buffer, fileName )
	{
		// Remove fileName if exist
		// ------------------------------------------------------------------------------------
		struct stat info;
		int res = stat(fileName.c_str(), &info);
		
		if (res != -1)	// If file exist... remove it!
		{
			if( remove( fileName.c_str() ) )
			{
				std::cerr << "Not possible to remove a file\n";
				exit(0);
			}
		}
		// ------------------------------------------------------------------------------------

		
		id = _id;
		task_id = _task_id;
		
		num_queues= 0;
		
		// Necessary for the double-linked list of the kvManager
		top = NULL;
		bottom = NULL;		
	
		// By default it is considered new until is assigned to a queue
		new_set = true;
	}
	
	KVSet::KVSet( size_t _id ,size_t _task_id, std::string fileName  ) : KVSetData( fileName )
	{
		id = _id;
		task_id = _task_id;
		
		
		num_queues= 0;
		
		// Necessary for the double-linked list of the kvManager
		top = NULL;
		bottom = NULL;		
		
		// By default it is considered new until is assigned to a queue
		new_set = true;
		
	}
	
	
	KVSet::~KVSet()
	{
	}

	void KVSetBase::add_task( size_t task_id )
	{
		
		tasks.insert( task_id );
		
		// Update information for sorting KVSets
		num_tasks++;
		min_task_id = *tasks.begin();
		
	}
	
	void KVSetBase::remove_task( size_t task_id )
	{
		assert( tasks.find( task_id ) != tasks.end() );
		tasks.erase( task_id );
		num_tasks--;
	}


	
	std::string KVSet::str()
	{
		std::ostringstream stream;
		// General information about the memory state
		
		stream << "[ "<< au::Format::int_string( id , 6 );
		stream <<  " / ";
		
		if( num_tasks == 0 )
			stream << au::Format::int_string( 0 , 6 );
		else
			stream << au::Format::int_string( min_task_id , 6 );
		stream <<  " ]";

		stream << "[";
		if ( isOnMemory() )
			stream << "M";
		else
			stream << " ";

		if (isOnDisk() )
			stream << "D";
		else
			stream << " ";
		
		if( useCounter == 0)
			stream << " ";
		else
			stream << "*";
		
		if( isTxtFormat() )
			stream << "T";
		else
			stream << " ";
		
		stream << "][";
		
		if( shouldBeRemoved() )
			stream << "R";
		else
			stream << " ";

		if( shouldBeFlushedToDisk() )
			stream << "F";
		else
			stream << " ";

		if( shouldBeReadFromDisk() )
			stream << "L";
		else
			stream << " ";
		
		
		stream << "]";

		
		stream << "[ Queues: ";
		for (size_t i = 0 ; i < queues.size() ; i++)
			stream << queues[i]->name << " ";
		stream << "]";
		
		stream << "[Tasks: " << num_tasks << "]";
		
		stream << " ";
		stream << "[" << au::Format::string( getNumKVs() ) << " key-values in "<< au::Format::string( getSize() ) << "]";
		stream << "[ " << getFormat().str() << " ]";
		
		return stream.str();
	}	

	
	
	
	int KVSet::compare( KVSet *other )
	{
		
		// If bloqued, higher preference
		if(  useCounter  && !other->useCounter )
			return 1;
		if(  !useCounter  && other->useCounter )
			return -1;
		
		// Now let's check witch is the next priority
		if( ( num_tasks == 0) && (other->num_tasks ==0 ) )
		{
			// TODO: Get a natural preference to sort this stuff
			return 0;
		}
		
		if( ( num_tasks == 0) && (other->num_tasks >0 ) )
			return -1;
		
		if( ( num_tasks > 0) && (other->num_tasks == 0 ) )
			return 1;
		
		
		if (min_task_id	< other->min_task_id)
			return 1;
		else
			return -1;
		
	}
	
	

	
	

	
}


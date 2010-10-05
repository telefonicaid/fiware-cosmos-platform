#ifndef SAMSON_KV_SET_H
#define SAMSON_KV_SET_H

#include <sys/types.h>
#include <sys/stat.h>           /* stat          */
#include <unistd.h>
#include <list>
#include <set>
#include <string>
#include <sstream>              /* ostringstream */

#include "samsonLogMsg.h"
#include "KVFormat.h"
#include "KVSetBuffer.h"
#include "Data.h"
#include "samson.pb.h"
#include "Format.h"



/**
 Class containing a list of tasks for KVSet.
 It also contains a lock mechanish since multiple elements can tasks over the same KVSet
 Example of this concurrency would be a task that is generating a subtask
 */

namespace ss {
	
	class KVFormat;
	class Data;
	class KVSetBuffer;
	class KVReader;
	class KVQueue;
	class KVStorage;
	
	/**
	 Base class with the real buffer information to hold data on memory
	 */
	
	class KVSetData
	{
				
		char * buffer;				//!< On memory buffer when used
		size_t bufferSize;			//!< Size of the buffer

		size_t header_size;				//!< Size of the header
		protocol::KVSetHeader header;	//!< Header with information about format /  size / num_kvs / etc.

		char *dataBuffer;			//!< Pointer to the dataSegment inside the buffer
		
		// System state information about this KVSet 
		
		std::string file_name;		//!< Full path for the saved file 
		bool on_memory;				//!< Is this set of KVs in memory
		bool on_disk;				//!< Is this set of KVs on disk
		
	public:
		
		/**
		 Constructor used when a KVSet is created by a particular task from a KVSetBuffer
		 */
		
		KVSetData(KVSetBufferBase* _buffer, std::string _file_name)
		{
			DataBuffer b = _buffer->getDataBuffer();

			buffer     = b.buffer;
			bufferSize = b.size;

			
			// Read the header
			header_size = *((size_t*) buffer);
			header.ParseFromArray(buffer + sizeof(size_t) , header_size);

			// Get the pointer to the current data
			dataBuffer = buffer + sizeof(size_t) + header_size;
			
			on_memory = true;	// By default it is in memory because it comes from a KVSetBuffer
			on_disk = false;
			
			file_name = _file_name;
		}

		/**
		 Constructor used when a KVSet if created from a file in "recovery mode"
		 */
		
		KVSetData( std::string _file_name )
		{
			file_name = _file_name;
			buffer =  NULL;
			
			// Get the size of this file
			struct stat file_status;
			stat(_file_name.c_str(), &file_status);
			bufferSize = file_status.st_size;
			
			// Read the header to get basic information
			size_t  nb;
			FILE*   file = fopen( file_name.c_str() , "r" );

			if (file == NULL)
			{
				LOG_ERROR(("error opening '%s' for reading: %s", file_name.c_str(), strerror(errno)));
			}
			else
			{
				nb = fread(&header_size, 1, sizeof(size_t), file);

				if (nb != sizeof(size_t))
				{
				}
				else
				{
					char buffer[10000];

					nb = fread(buffer, 1, header_size, file);
					if (nb != header_size)
					{
						LOG_ERROR(("read only %d bytes (expected %d)", nb, header_size));
					}
					else
					{
						header.ParseFromArray( buffer , header_size );
			
						on_memory = false;
						on_disk = true;
					}
				}

				fclose(file);
			}
		}
		
		/**
		 Function to flush content from memory to disk ( if there is anything unsaved )
		 */
		
		void flushToDisk();		// Flush content to disk of the memory buffer to disk
		
		/*
		 Function to load saved content from disk to memory
		 */
		
		void loadFromDisk();	// Load from disk 
		
		/*
		 Function to free this buffer from memory.
		 Return value: bytes free from memory
		 */
		
		size_t freeBuffer( );		// Free memory
		
		
		/**
		 Debuging function to show information
		 */
		
		std::string strBufferInfo()
		{
			// Complete info
			::std::ostringstream o;
			
			o << "[KVS: " << header.num_kvs() << " S: " << au::Format::string( bufferSize );
			
			if( on_memory )
				o << "M";
			else
				o << " ";
			
			if( on_disk )
				o << "D";
			else
				o << " ";
			
			o << "]";
			
			return o.str();		
		}
				
		size_t getSize()
		{
			return bufferSize;
		}
		
		size_t getNumKVs()
		{
			return header.num_kvs();
		}
		
		KVFormat getFormat()
		{
			KVFormat format;
			format.keyFormat = header.format().key();
			format.valueFormat = header.format().value();
			return format;
		}
		
		bool isOnMemory()
		{
			return on_memory;
		}
		
		bool isOnDisk()
		{
			return on_disk;
		}
		
		bool isTxtFormat()
		{
			return header.txtformat();
		}
	
		/** 
		 Get a vector of KVs to use in operations 
		 */
		
		KVVector* newKVVectorStandart()
		{
			KVVector *tmp = new KVVector( getNumKVs() );

			// Initial offset inside the buffer to skip the header
			size_t offset = sizeof( size_t ) + header_size;
			size_t kv_index = 0;
			
			while( offset < bufferSize )
			{
				size_t key_size;
				offset += staticVarIntParse( buffer+offset , &key_size );	//Get size of the key element
				
				tmp->kvs[kv_index].key = buffer+offset;
				offset += key_size;
				
				size_t value_size;
				offset += staticVarIntParse( buffer+offset , &value_size );	//Get size of the key element
				
				tmp->kvs[kv_index].value = buffer+offset;
				offset += value_size;
				
				
				tmp->kvs[kv_index].key_size = key_size;
				tmp->kvs[kv_index].value_size = value_size;
				
				kv_index++;
			}
			
			assert( kv_index == getNumKVs() );
			
			
			return tmp;
		
		}

		KVVector* newKVVectorTxtFormat()
		{
			assert( getFormat().keyFormat == "system.String" );
			assert( getFormat().valueFormat == "system.Void" );

			KVVector *tmp = new KVVector( getNumKVs() );

			size_t kv_index = 0 ;
			size_t offset = sizeof( size_t ) + header_size;
			size_t pos_init = offset;
			for (size_t pos = offset ; pos < bufferSize ; pos++)
			{
				if( buffer[pos] == '\0' )
				{					
					tmp->kvs[kv_index].key = &buffer[pos_init];
					tmp->kvs[kv_index].key_size = pos - pos_init;

					tmp->kvs[kv_index].value = NULL;
					tmp->kvs[kv_index].value_size = 0;
					
					kv_index++;
					 
					pos_init = pos+1;
				 
				}				
			}
			
			assert( kv_index == getNumKVs() );
			
			return tmp;
		}
		
		KVVector* newKVVector()
		{
			
			if( isTxtFormat() )
				return newKVVectorTxtFormat();
				
			// By default standart mode
			return newKVVectorStandart();
		}
		
		
		
		
	
		
	private:
		friend class KVManager;
		
		void setOnDisk()
		{
			on_disk = true;
		}

		void setOnMemory()
		{
			on_memory = true;
		}
		
		
	};
	
	
	/**
	 Base class for KVSet to indicate what Tasks are interested in this KVSet
	 This help sorting KVSets for read/write to disk
	 */
	
	class KVSetBase
	{
		std::set<size_t> tasks;		// List of tasks that want to use this element in order
		
	public:
		
		// Information for comparison ( thread safe )
		size_t num_tasks;			// Number of task interested in this KVSet (on memory)
		size_t min_task_id;			// Min id of all the tasks
		
	public:
		
		KVSetBase();

	private:
		
		friend class KVManager;		// Managed from KVManager with a common "lock"
		friend class KVSetsManager;		// Used througth KVManager
		friend class KVManagerSets;
		
		/**
		 Add a task to this KVSet.
		 This means that this taks wants to use this KVSet
		 */
		
		void add_task( size_t task_id);
		void remove_task( size_t task_id );

	protected:
		
		/**
		 Use counter is the number of element using this KVSet
		 If it is > 0, it cannot be removed from memory
		 */
		
		int useCounter;			//!< Counter the number of tasks using this KVSet
		
		/**
		 Note:
		 If an element is not in a queue, it is not currently used, and there is no task waiting for it
		 it can be completelly removed 
		 */
		
		void retainToUse()
		{
			useCounter++;
		}
		
		void releaseToUse()
		{
			useCounter--;
		}
		
		
		
	};	

	/** 
	 Main data element of the SAMSON platform
	 It contains a list of KV pairs in a particular format
	 There are two type of keyValues:
		- normal: key-value , key-value, key-value , ...
		- hash: hash-length-key-value , hash-length-key-value , ...
	 
	 */

	
	class KVSet : public KVSetBase , public KVSetData
	{
		
	public:

		size_t id;					//!< Unique identifier of the KVSet
		size_t task_id;				//!< Task that created this KVSet ( for log-based crash recovery tool )
		
		// Double-linked list of KVSets
		KVSet *top;					//!< Pointer to the a higher preference KVSet
		KVSet *bottom;				//!< Pointer to a lower priority KVSet
		
		std::vector<KVQueue*> queues;	//!< KVQueues where this KVSet is stored
		bool new_set;					//!< Flag to indicate that this KVSet has been just created and should not be removed
		
	public:
		int num_queues;					// Number of queues that this KVSet is included to
		
	private:
		
		friend class KVManager;
		friend class KVSetsManager;
		friend class KVManagerSets;

		
		KVSet( size_t id , size_t task_id, std::string fileName,   KVSetBufferBase * buffer );								
		KVSet( size_t id , size_t task_id, std::string fileName );
		
	public:
		virtual ~KVSet();

		/**
		 Function to compare in order to sort a specific order
		 */
		
		int compare( KVSet *other );

		/**
		 Help functions to work with KVSets
		 */
		
		bool shouldBeRemoved()
		{
			return ( !new_set && (useCounter==0) && (num_queues==0) && (num_tasks==0) );
		}
		
		bool canBeFree()
		{
			return ( (useCounter==0) && isOnMemory() && isOnDisk() );
		}
					
		bool shouldBeFlushedToDisk()
		{
			return ( !shouldBeRemoved() && isOnMemory() && !isOnDisk() );
		}
		
		bool shouldBeReadFromDisk()
		{
			//return( !shouldBeRemoved() && on_disk && !on_memory && (num_tasks>0) );
			return( !shouldBeRemoved() && isOnDisk() && !isOnMemory() );
		}
		
		size_t getId()
		{
			return id;
		}
		
		std::string str();
		

		/**
		 Queue management
		 */
		
	private:
		friend class KVQueue;
		
		void addQueue( KVQueue *queue );	
		void removeQueue( KVQueue *queue );
		
		void notifyFlushKVSetToDisk();
		void notifyReadKVSetFromDisk();
		void notifyFreeKVSet();
	};
}

#endif

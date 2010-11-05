#ifndef _H_KVWriterToBuffer
#define _H_KVWriterToBuffer

/* ****************************************************************************
*
* FILE                 KVWriterToBuffer.h - 
*
*/
#include <iostream>					// std::cout
#include <vector>					// std::vector
#include <set>						// std::set
#include <algorithm>                // std::sort

#include "coding.h"					// ss_hg, ss_hg_size, etc..
#include "samson/KVWriter.h"		// KVWriter
#include "Lock.h"					// au::Lock
#include "BufferSinkInterface.h"	// ss::BufferSinkInterface
#include "MemoryManager.h"			// ss::MemoryManager


namespace ss {


	/**
	 Simple Buffer information
	 */
	
	class SimpleBuffer
	{
	public:
		char *data;
		size_t size;
		
		
		SimpleBuffer()
		{
			data = NULL;
			size = 0;
		}
		
		SimpleBuffer( char *_data , size_t _size)
		{
			data = _data;
			size = _size;
		}
		
		SimpleBuffer split( int i , int num)
		{
			assert( (size_t)num > size); // Otherwise it is not divisible
			size_t split_size = size/num;
			
			SimpleBuffer tmp( data + i * tmp.size , split_size);
			return tmp;
		}
		
		char *end()
		{
			return data + size;
		}
		
		char *begin()
		{
			return data;
		}
		
	};
	

	
	
	bool compare_hg_size_offset( hg_size_offset a , hg_size_offset b )
	{
		return (a.hg < b.hg);
	}
	
	
	class KVSimpleWriter 
	{
		
		SimpleBuffer _buffer;
		
		size_t _offset;			// Position inside the buffer where we write the next KV
		hg_info *_info;			// Number of key-values ( stored at the end of the available memory space)
		
		hg_size_offset* _kvs;	// Information about the emitted kvs ( negative offsets )
		
	public:
		
		KVSimpleWriter( SimpleBuffer buffer )
		{
			// Keep the buffer of available memory space
			_buffer = buffer;

			// Point to the last position of the vector to store the total size and number of key-values emitted
			_info = (hg_info*) (_buffer.end() - sizeof( hg_info) );

			// Pointing to the place where we save this vector
			_kvs = (hg_size_offset*) (_buffer.end() - sizeof( hg_info ) - sizeof(hg_size_offset));
			
			// Reset the buffer
			reset();
			
			
		}
		
		void reset()
		{
			_info->kvs = 0;
			_info->size = 0;
			_offset = 0;
		}
		
		// KVWriter interface
		
		bool emit( ss_hg hg , char *tmpBuffer , size_t size )
		{
			
			if ( ( _offset + size + (_info->kvs+1)*sizeof(hg_size_offset) + sizeof( hg_info ) ) > _buffer.size )
				return false;
			
			// Store the new key-value
			memcpy(_buffer.data + _offset, tmpBuffer, size);		// Copy the data to the buffer
			
			// Store information about this new key-value in an ad-hoc reverse vector at the end of the memory space
			hg_size_offset *kv = _kvs - _info->kvs; // Reverse order in key-values
			kv->hg = hg;
			kv->size = size;
			kv->offset = _offset;
			
			// Update the total number of kvs and total size
			_info->kvs++;
			_info->size+=size;
			
			// Update the offset in the available memory space
			_offset += size;

			return true;
			
		}
		
		/**
		 Function called when the buffer is full.
		 The Buffer is reset after this call returns
		 */
		
		void sort()
		{
			
			// Sort the key-values emitted ( we only sort the reference vector ) by hash group
			
			hg_size_offset * tmp = (hg_size_offset*) ( _buffer.end() - sizeof( hg_info ) - (_info->kvs)*sizeof( hg_size_offset ) );
			
			/*
			for (size_t i = 0 ; i <_info->kvs ; i++)
				std::cout << tmp[i].hg << " ";
			std::cout << std::endl;
			*/
			
			std::sort( tmp , tmp + _info->kvs , compare_hg_size_offset );
			
			/*
			 for (size_t i = 0 ; i <_info->kvs ; i++)
				std::cout << tmp[i].hg << " ";
			std::cout << std::endl;
			*/
		}
		
		/**
		 Get a buffer with Key-values
		 */
		
		static Buffer * getBuffer( SimpleBuffer simple_buffer )
		{
			// Point to the last position of the vector to store the number of key-values emitted
			hg_info* total_info = (hg_info*) ( simple_buffer.end() - sizeof( hg_info) );

			size_t total_size = KV_HASH_GROUP_VECTOR_SIZE + total_info->size;
			Buffer * buffer = MemoryManager::shared()->newBuffer( total_size );

			// Pointer to the initical vector of hash-group information
			hg_info *info = (hg_info*) buffer->getData();

			// Pointer to data in the generated buffer
			char *bufferData = buffer->getData();
			
			// Write the total size and number of kvs
			info[0] = *total_info;

			// Write the key-values in the buffer
			
			hg_size_offset* kvs = (hg_size_offset*) ( simple_buffer.end() - sizeof( hg_info ) - total_info->kvs * ( sizeof(hg_size_offset)));

			size_t hg = 0;
			info[hg+1].kvs = 0;
			info[hg+1].size = 0;
			
			size_t offset = KV_HASH_GROUP_VECTOR_SIZE;
			
			for (size_t i = 0 ; i < total_info->kvs ; i++)
			{
				if( kvs[i].hg > hg )
				{
					hg++;
					info[hg+1].kvs = 0;
					info[hg+1].size = 0;
				}
				
				memcpy(bufferData+offset, simple_buffer.data+kvs[i].offset, kvs[i].size);
				info[hg+1].kvs++;
				info[hg+1].size += kvs[i].size;
				
				// Update the offset in the buffer data
				offset+= kvs[i].size;
			}
			
			// Make sure the size is coherent with the allocated buffer
			assert( offset == total_size );
			
			// Rest of unused hash-group
			while( hg < KV_NUM_HASHGROUPS )
			{
				hg++;
				info[hg+1].kvs = 0;
				info[hg+1].size = 0;
			}
			
			return buffer;
		}
		
		
	};	


	
	class KVSerializer
	{
		
	public:
		
		// Temporal buffer to serialize key-values
		char *data;
		size_t size;
		
		KVSerializer()
		{
			//Init the temporal buffer
			data = (char*) malloc( KV_MAX_SIZE );		
		}
		
		~KVSerializer()
		{
			free(data);
		}

		void serialize( DataInstance& key , DataInstance &value )
		{
			size_t key_size = key.serialize( data );
			size_t value_size = value.serialize( data + key_size );
			
			size = key_size + value_size;
		}
			
		
	};
	


/*	
	
	typedef struct
	{
		size_t offset;		// Position in the buffer where this key-value is stored
		unsigned int size;	// Size of the key-value in bytes
		ss_hg hashGroup;	// HashGroup of the key value
	} KVHashGroup;

	bool KVHashGroupSorter(KVHashGroup *a, KVHashGroup *b);

	class KVWriterGeneral;
	class KVWriterToBuffer : public KVWriter
	{
		KVWriterGeneral* generalWriter;		// Pointer to the general writing
		int server;							// Identifier of the server that corresponds to me
		
		// Internal buffer to store key-values
		char * _data;
		size_t _size;
		
		// Vector of generated key-values
		KVHashGroup* _kvs;
		size_t _num_kvs;
		
		// Vector of pointers of the structures
		KVHashGroup** __kvs;
		
		char *_tmp_data;
		
		
	public:
		
		KVWriterToBuffer( KVWriterGeneral *_generalWriter  , int _server )
		{
			generalWriter = _generalWriter;
			server = _server;
			
			// Init the buffer data
			_size = 0;
			_data = (char*) malloc(KV_BUFFER_SIZE);
			
			// Init the kvs buffer
			_kvs = (KVHashGroup*) malloc(KV_BUFFER_MAX_NUM_KVS *sizeof(KVHashGroup) );
			_num_kvs = 0;
			
			// Init the vector of pointsd
			__kvs = (KVHashGroup**) malloc(KV_BUFFER_MAX_NUM_KVS * sizeof(KVHashGroup*) );
			
			// Init the tmp buffer to serialize key-values
			_tmp_data = (char*) malloc( KV_MAX_SIZE );
		}
		
		~KVWriterToBuffer()
		{
			free( _data );
			free( _tmp_data );
		}
		
		virtual void emit(DataInstance* key, DataInstance* value)
		{
			size_t key_size = key->serialize(_tmp_data);
			size_t value_size = value->serialize(_tmp_data+key_size);
			size_t total_size = key_size + value_size;
			
			if( (_size + total_size) > KV_BUFFER_SIZE )
				processBuffer();
			
			// Store the new key-value
			_kvs[_num_kvs].offset = _size;
			_kvs[_num_kvs].size = total_size;
			_kvs[_num_kvs].hashGroup = key->hash(KV_NUM_HASHGROUPS);
			__kvs[_num_kvs] = &_kvs[_num_kvs];
			_num_kvs++;
			
			// Copy the new key-value to the buffer
			memcpy(_data+_size, _tmp_data, total_size);
			_size += total_size;
			
			
			if( _num_kvs >= KV_BUFFER_MAX_NUM_KVS)
				processBuffer();
			
		}
		
		void processBuffer();
		
		virtual void close()
		{
			// Process the last piece of data ( if any )
			if( _size > 0 )
				processBuffer();
		}
		
	};
	
	

	
	class KVWriterGeneral : public KVWriter
	{
		int num_servers;
		std::vector<KVWriterToBuffer*> buffers;		// Real buffers

		// Buffers of data generated pendign to be removed
		std::set<Buffer*> generatedBuffers;	
		
		au::Lock lock;

		
		BufferSinkInterface* _interface;
		
	public:

		KVWriterGeneral( int _num_servers , BufferSinkInterface* interface )
		{
			_interface = interface;
			
			num_servers = _num_servers;
			for (int i = 0 ; i < num_servers ; i++)
				buffers.push_back( new KVWriterToBuffer( this , i ) );
		}

		
		virtual void emit(DataInstance* key, DataInstance* value)
		{
			int server = key->hash( num_servers );
			buffers[server]->emit( key , value );
		}
		
		virtual void close()
		{
			for (int i = 0 ; i < num_servers ; i++)
				buffers[i]->close();
		}
		
		
		
		void processBuffer( Buffer *b )
		{
			std::cout << "Global writer receive a packet for server " << b->server << " with " << b->getSize() << " bytes\n";

			if( _interface )
			{
				lock.lock();
				_interface->newBuffer( b );				
				generatedBuffers.insert( b );
				lock.unlock();
			}
		}
		
		
		void removeGeneratedBuffer( Buffer *b )
		{
			lock.lock();
			
			std::set<Buffer*>::iterator i = generatedBuffers.find(b);
			assert( i != generatedBuffers.end() );
			generatedBuffers.erase( i );
			MemoryManager::shared()->destroy( b );
			
			lock.unlock();
			
			
		}
		
		
	};
 */
	
}
#endif

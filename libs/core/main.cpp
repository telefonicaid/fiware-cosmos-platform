

#include <iostream>
#include "ModulesManager.h"
#include "samson/Data.h"
#include <vector>

#define KV_BUFFER_SIZE			1024*1024*256
#define KV_BUFFER_MAX_NUM_KVS	 1024*1024*64
#define KV_MAX_SIZE				 	  64*1024
#define KV_NUM_HASHGROUPS			  64*1024	

#define KV_HASH_GROUP_VECTOR_SIZE	(sizeof(HashGroupInfo)*KV_NUM_HASHGROUPS)

namespace ss {


	typedef struct
	{
		size_t size;
	} HashGroupInfo;


	// Buffer to send data across SAMSON platform
	//
	// [ HashGroupInfo vector ][ Key - vale data ]
	
	
	/**
	 KVHashGroup:		Structure to emit key-values and sort latter by hashGroup
	 */
	
	typedef unsigned short ss_hg;
	
	typedef struct
	{
		size_t offset;				// Position in the buffer where this key-value is stored
		unsigned int size;			// Size of the key-value in bytes
		ss_hg hashGroup;	// HashGroup of the key value
	} KVHashGroup;
	
	bool KVHashGroupSorter(KVHashGroup *a, KVHashGroup *b) {
		return a->hashGroup < b->hashGroup;
	}
	
	
	class KVFileSaver
	{
		std::vector<char*> buffers;	// Store of buffers received
		size_t total_size;

		HashGroupInfo *hgs;
		
	public:
		
		KVFileSaver()
		{
			hgs = (HashGroupInfo*) malloc( KV_NUM_HASHGROUPS * sizeof(HashGroupInfo) );
		}
		
		~KVFileSaver()
		{
			free(hgs);
		}
		
		void add( char*b )
		{
			buffers.push_back( b );

			// Monitorize the total size
			HashGroupInfo *_hgs = (HashGroupInfo *)b;
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				hgs[i].size += _hgs[i].size;
				total_size += _hgs[i].size;
			}
		}
		
		void save( std::string fileName )
		{
			char *buffer = (char *) malloc( total_size );
			size_t bufferSize = 0;
			
			size_t *offset = (size_t*) malloc( buffers.size() * sizeof(size_t));
			for (unsigned int f = 0 ; f < buffers.size() ; f++ )
				offset[f] = KV_HASH_GROUP_VECTOR_SIZE;
		
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				for (unsigned int f = 0 ; f < buffers.size() ; f++ )
				{
					char *b = buffers[f];
					size_t size = ((HashGroupInfo*)b)[i].size;
					
					memcpy(buffer+bufferSize, b+offset[f], size);
					offset[f]+=size;
					bufferSize+=size;
					
				}
			}
			assert( bufferSize == total_size );
			
			
			std::cout << "Saving a file of size " << total_size << " bytes( pluss "<< KV_HASH_GROUP_VECTOR_SIZE << " bytes)\n";
			
			
			
			// Simple test to read the first hash-group
			{
				ss::system::UInt a;
				ss::system::Int32 b;
				size_t s = hgs[0].size;
				size_t offset = 0;
				std::cout << "Reading ket-values of HG 0 with " << s << " bytes\n";
				while (offset<s)
				{
					offset+= a.parse( buffer + offset );
					offset+= b.parse( buffer + offset );
					
					std::cout << "OFF:" << offset << " -> " << a.str() << "\n";
				}
			}
			
		}
		
		
		
		
		
	};
	
	class KVWriterToBuffer
	{
		
		// Internal buffer to store key-values
		char * _data;
		size_t _size;
		
		// Vector of generated key-values
		KVHashGroup* _kvs;
		size_t _num_kvs;
		
		// Vector of pointers of the structures
		KVHashGroup** __kvs;
		
		char *_tmp_data;
		
		KVFileSaver tmp;
		
	public:
		
		KVWriterToBuffer( )
		{
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
		
		void processBuffer()
		{
			std::cout << "Porcessing buffer of size " << _size << " bytes and " << _num_kvs << " key-values\n";
			
			// Sort the buffer so that key-values with the same data-group are toguether
			std::sort( __kvs , __kvs+_num_kvs , KVHashGroupSorter);
			

			// Prepare the buffer to "send"
			char *buffer = (char*) malloc( KV_HASH_GROUP_VECTOR_SIZE + _size );
			
			HashGroupInfo *hgs = (HashGroupInfo*)buffer;
			char *dataBuffer = buffer + KV_HASH_GROUP_VECTOR_SIZE;
			size_t dataBufferSize = 0;
			
			size_t pos = 0;
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				size_t total_size = 0 ;
				size_t total_num_kvs = 0;
				
				
				while( (pos < _num_kvs) && (__kvs[pos]->hashGroup == i) )
				{
					memcpy( dataBuffer + dataBufferSize  , _data + __kvs[pos]->offset, __kvs[pos]->size);
					dataBufferSize += __kvs[pos]->size;
					
					total_size += __kvs[pos]->size;
					total_num_kvs++;
					
					pos++;

				}
				
				hgs[i].size = total_size;
				//hgs[i].num_kvs = total_num_kvs;
			}

			assert( dataBufferSize == _size );	// Make sure we have copied all key-values
			
			
			tmp.add(buffer);
			
			// Do something witht he buffer
			_size = 0;
			_num_kvs =0 ;
		}
		
		virtual void close()
		{
			// Process the last piece of data ( if any )
			if( _size > 0 )
				processBuffer();
			
			
			tmp.save("");
		}
		
	};
	
	
}


int main( int argc , char *argv[] )
{
	std::cout << "Test\n";
	
	ss::ModulesManager mm;

	
	ss::Data *data = mm.getData("example.example");
	
	if( data )
		std::cout << data->help();
	
	
	ss::system::UInt a;
	ss::system::Int32 b;
	ss::KVWriterToBuffer output;
	
	for (size_t i = 0 ; i < 10000000 ; i++)
	{
		a = i;
		b = i;
		
		// Emit this key-value to a buffer
		output.emit( &a , &b );
	}
	output.close();
	
}
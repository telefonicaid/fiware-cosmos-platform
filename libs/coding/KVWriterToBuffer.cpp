#include <assert.h>						// assert()
#include <algorithm>                    // std::sort

#include "KVWriterToBuffer.h"			// Own interface
#include "MemoryManager.h"				// ss::MemoryManager

/*

namespace ss {

	bool KVHashGroupSorter(KVHashGroup *a, KVHashGroup *b) {
		return a->hashGroup < b->hashGroup;
	}
	
	
	void KVWriterToBuffer::processBuffer()
	{
		std::cout << "Porcessing buffer of size " << _size << " bytes and " << _num_kvs << " key-values\n";
		
		// Sort the buffer so that key-values with the same data-group are toguether
		std::sort( __kvs , __kvs+_num_kvs , KVHashGroupSorter);
		
		// Prepare the buffer to "send"
		Buffer *buffer = MemoryManager::shared()->newPrivateBuffer( KV_HASH_GROUP_VECTOR_SIZE + _size  );
		
		ss_hg_size *hgs = (ss_hg_size*)buffer->getData();
		char *dataBuffer = buffer->getData() + KV_HASH_GROUP_VECTOR_SIZE;
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
			
			// Set size
			hgs[i] = total_size;
		}
		
		assert( dataBufferSize == _size );	// Make sure we have copied all key-values
		
		
		// Do something witht he buffer
		buffer->server = server;
		generalWriter->processBuffer( buffer);
		
		_size = 0;
		_num_kvs =0 ;
	}
}
 */

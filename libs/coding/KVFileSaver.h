

#ifndef _H_KVFILESAVER
#define _H_KVFILESAVER

#include <vector>					// std::vector
#include "coding.h"					// 
#include "BufferSinkInterface.h"	// ss::BufferSinkInterface


namespace ss {

	class KVFileSaver : public BufferSinkInterface
	{
		std::vector< Buffer *> buffers;	// Store of buffers received
		size_t total_size;
		
		ss_hg_size *hgs;
		
	public:
		
		KVFileSaver()
		{
			hgs = (ss_hg_size*) malloc( KV_NUM_HASHGROUPS * sizeof(ss_hg_size) );
			for(int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
				hgs[i]= 0;
			total_size = 0;
		}
		
		~KVFileSaver()
		{
			free(hgs);
		}
		
		virtual void newBuffer( Buffer * buffer )
		{
			buffers.push_back( buffer );
			
			// Monitorize the total size
			ss_hg_size *_hgs = (ss_hg_size *)buffer->getData();
			for (int i = 0 ; i < KV_NUM_HASHGROUPS ; i++)
			{
				hgs[i] += _hgs[i];
				total_size += _hgs[i];
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
					char *b = buffers[f]->getData();
					size_t size = ((ss_hg_size*)b)[i];
					
					memcpy(buffer+bufferSize, b+offset[f], size);
					offset[f]+=size;
					bufferSize+=size;
					
				}
			}
			assert( bufferSize == total_size );
			
			std::cout << "Saving a file of size " << total_size << " bytes( pluss "<< KV_HASH_GROUP_VECTOR_SIZE << " bytes)\n";

			FILE *file = fopen(fileName.c_str(), "w");
			fwrite(hgs, 1, KV_HASH_GROUP_VECTOR_SIZE, file);
			fwrite(buffer, 1, bufferSize, file);
			fclose( file );
			
			
		}
	};
}
#endif

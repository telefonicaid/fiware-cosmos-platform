#ifndef _H_PROCESS_OPERATION_FRAMEWORK
#define _H_PROCESS_OPERATION_FRAMEWORK

#include <algorithm>                        // sort
#include <cstdlib>                          // ...

#include "ProcessWriter.h"					// ss::ProcessWriter
#include "samson.pb.h"						// ss::network::,,,
#include "coding.h"							// FileHeader, 
#include "OperationFramework.h"				// Own interface

#include <algorithm>

namespace ss {
	
	
	
	/* Vector key-values to sort and process all input channels */
	
	class KVInputVector
	{
		
	public:
		
		KV *kv;							// Dynamic Vector of KV elements
		KV ** _kv;						// Dynamic Vector with pointers to kv
		
		size_t max_num_kvs;				// Allocation size
		size_t num_kvs;					// Real number of kvs in the vectors
		
		DataSizeFunction keySize;		// Function to get the size of a particular value of the key ( common to all inputs )
		DataSizeFunction *valueSize;	// Function to get the size of a partiuclar value of the value ( different for each input )
		
		OperationInputCompareFunction compare;
		
		int num_inputs;
		
		KVInputVector( int _num_inputs )
		{
			num_inputs = _num_inputs;
			
			_kv = NULL;
			kv  = NULL;
			max_num_kvs = 0;
			num_kvs = 0;
			
			valueSize = (DataSizeFunction *) malloc( sizeof( DataSizeFunction) * num_inputs );
		}
		
		~KVInputVector()
		{
			if( _kv )
				free( _kv );
			if( kv ) 
				free ( kv );
		}
		
		// For each hash group, prepare the vector
		
		void prepareInput( size_t _max_num_kvs )
		{
			if( _max_num_kvs > max_num_kvs )
			{
				if( _kv )
					free( _kv );
				if( kv ) 
					free ( kv );

				// Set a new maximum number of kvs
				max_num_kvs = _max_num_kvs;
				
				_kv = (KV**) malloc( sizeof(KV*) * _max_num_kvs );
				kv  = (KV*) malloc( sizeof(KV) * max_num_kvs );
				
			}
			
			// Set the pointers to internal structure ( to sort again )
			for (size_t i = 0 ; i < max_num_kvs ; i++)
				_kv[i] = &kv[i];
			
			num_kvs = 0;
		}
		
		void addKVs( ProcessSharedFile& file )
		{
			int input = file.header->input;
			
			// Get the rigth size function
			DataSizeFunction _valueSize = valueSize[input];

			// Get the pointer to data
			char *data = file.data + file.offset;

			// Get the info we should read
			FileKVInfo info = file.info[ file.hg ];

			
			// Local offset
			size_t offset = 0;
			
			// Process a set of key values
			for (size_t i = 0 ; i < info.kvs ; i++)
			{
				kv[ num_kvs ].key = data + offset;
				
				offset += keySize( data + offset );
				
				kv[ num_kvs ].value = data + offset;
				
				offset += _valueSize( data + offset );

				kv[num_kvs].input = input;
				
				num_kvs++;
			}

			// Make sure the parsing is OK!
			assert( offset == info.size );
			
			// update the  file for the next round
			file.hg++;
			file.offset += info.size;
			
		}
		
		
		// global sort function key - input - value
		void sort()
		{
			std::sort( _kv , _kv + num_kvs , compare );
		}
		
	};
	
	class Process;
	
	class ProcessOperationFramework : public OperationFramework
	{
		
		Process *process;
		Operation *operation;
		
		Environment * environment;
		
	public:
		
		ProcessOperationFramework( Process *_process , network::ProcessMessage m  );
		~ProcessOperationFramework();
		
		void flushOutput( WorkerTaskItem *taskItem );
		
		
		void run();
		
		void runParserOut();		
		void runMap();		
		void runReduce();
	};	
}

#endif

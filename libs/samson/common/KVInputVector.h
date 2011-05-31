#ifndef _H_KV_INPUT_VECTOR
#define _H_KV_INPUT_VECTOR

#include "samson/common/coding.h"				// KV*
#include "samson/module/Operation.h"	// 	OperationInputCompareFunction

namespace samson
{
	/*
     Class to process alll input key-values for a map, reduce or parseOut operation
	 Vector key-values to sort and process all input channels 
	 */
	
	class KVInputVector
	{
		
	public:
		
		KV *kv;         // Dynamic Vector of KV elements
		KV ** _kv;      // Dynamic Vector with pointers to kv
		
		size_t max_num_kvs;// Allocation size
		size_t num_kvs;// Real number of kvs in the vectors
		
		DataSizeFunction keySize;       // Function to get the size of a particular value of the key ( common to all inputs )
		DataSizeFunction *valueSize;    // Function to get the size of a partiuclar value of the value ( different for each input )
		
		OperationInputCompareFunction compare; // Unique funciton to compare two key-values ( for any input )
		
		int num_inputs;// Number of input channels ( 1 in maps and parseOut , N in reduce operations )
		
		KVInputVector( Operation* operation );
		KVInputVector( int _num_inputs );
		~KVInputVector();
		
		void prepareInput( size_t _max_num_kvs );
		
		void addKVs( ProcessSharedFile& file );
		
		void addKVs(int input , KVInfo info , char *data );
        
		// global sort function key - input - value used in reduce operations
		void sort();
		
	};
	
}

#endif

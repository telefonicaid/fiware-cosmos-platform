#ifndef SAMSON_KV_READER_COMPARE_H
#define SAMSON_KV_READER_COMPARE_H

#include "KV.h"                  /* KV                                       */



namespace ss {

	#pragma mark Key Sort Functions

	inline int compareFullByKey(KV* kv1, KV* kv2)
	{
		// Full comparison by key necessary in reduce operations to group multiple sources of key-values
		
		if( kv1->key_size < kv2->key_size )
			return -1;
		if( kv1->key_size > kv2->key_size )
			return 1;
		
		for (size_t i = 0 ; i < kv1->key_size ; i++)
		{
			if( kv1->key[i] != kv2->key[i] )
				return kv1->key[i]-kv2->key[i];
		}
		return 0;
	}		
	
	inline bool compareByKey(KV*kv1 , KV*kv2)
	{
		return ( compareFullByKey(kv1,kv2) < 0);
	}
}

#endif

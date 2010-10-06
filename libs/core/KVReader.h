#ifndef SAMSON_KV_READER_H
#define SAMSON_KV_READER_H

/*
 *  KVReader.h
 *  mr_platform
 *
 *  Created by ANDREU URRUELA PLANAS on 4/12/10.
 *  Copyright 2010 TID. All rights reserved.
 *
 */
#include <algorithm>
#include <vector>                /* vector                                   */

#include <samson/KVVector.h>     /* KVVector                                 */
#include <samson/Data.h>                /* Data                                     */
#include <samson/KVFormat.h>            /* KVFormat                                 */



namespace ss {

	
	/**
	 Class to control a KVSet
	 It allows to sort and count the number of elements with the same key
	 */
	
	class KVSet;
	class KVSetVector;

	class KVReader
	{
				
	protected:
		KVSetVector* setGroup;

	public:
		KVReader( KVSetVector* setGroup );
		~KVReader();
		
		KV **kvs;				// Array of pointers to the KVs
		size_t num_kvs;			// Number of key-values
		
		std::vector<KVVector*> kvVectors;	// Vector of KVVector ( one for each KVSet )
		
		bool sort_by_value;		//!< Flag to indicate if it is necessary to sort by value
			
		Data *key;
		Data *value;

		// Special function provided by the data type to compare by value if necessary ( and possible )
		DataCompareFunction compareByValueFunction;
		
	public:

		/**
		 Sort grouping kvs with the same key toguether
		 */
		
		void sort_by_key();

		/** 
		 Number of key-values with the same key ( sort by value if indicated )
		 */
		
		size_t getNumKVS_WithSameKey( size_t index );
	};
}

#endif

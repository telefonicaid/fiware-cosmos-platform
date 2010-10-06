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

#include "kvVector.h"            /* KVVector                                 */
#include "Data.h"                /* Data                                     */



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
	
	/** 
	 Class used to hold a set of KVSets with common key-type
	 */
	
	class KVMultipleReader
	{
		std::vector<KVReader*> vectors;

		int num;				// Number of sets at the input ( max 100 )
		size_t index[100];		// Index for each input
		bool participate[100];	// Flag to indicate that this set participate in this round of sets
		bool finish[100];		// Flag to indicate that this input is finish (no more kvs)

		KVSET sets[100];		// Used for output
		
	public:
		
		KVFormat format;		// Format for the key-value
		
		KVMultipleReader( std::vector<KVSetVector*> setsGroups );
		
		~KVMultipleReader()
		{
			for (size_t i = 0 ; i < vectors.size() ; i++)
				delete vectors[i];
			vectors.clear();
		}

		/**
		 Get the full set of key-values without sorting for map operations
		 */
		
		KVSET* getFullSets()
		{
			for (size_t i = 0 ; i < vectors.size() ; i++)
			{
				sets[i].num_kvs = vectors[i]->num_kvs;
				sets[i].kvs = vectors[i]->kvs;
			}
			
			return sets;
		}
		

		/**
		 Operations needed for getting sets of key-values grouped by key and sorted by value
		 */
		
		void initReduceSets()
		{
			for (size_t i = 0 ; i < vectors.size() ; i++)
			{
				vectors[i]->sort_by_key();
				index[i] = 0;
			}
			
		}
		
		KVSET* getNextReduceSets();
		
		
		
	};
							 
	
	
}

#endif

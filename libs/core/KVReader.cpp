/* ****************************************************************************
*
* FILE              KVReader.cpp
*
*  Created by ANDREU URRUELA PLANAS on 4/12/10.
*  Copyright 2010 TID. All rights reserved.
*
*/
#include "Data.h"
#include "Module.h"
#include "KVSet.h"
#include "ModulesManager.h"
#include "KVQueue.h"
#include "Lock.h"                /* Lock                                     */
#include "KVReaderCompare.h"     /* compareFullByKey, compareByKey           */
#include "KVReader.h"            /* Own interface                            */



#pragma mark Auxiliar functions to get the bounds of a part of the vector
#pragma mark -


namespace ss {
				
   	#pragma mark KVReader
	#pragma mark -
	
	KVReader::KVReader( KVSetVector* _setGroup  )
	{
		// Keep a poitner to the set
		setGroup = _setGroup;
		
		key = ModulesManager::shared()->getData( setGroup->format.keyFormat );
		value = ModulesManager::shared()->getData( setGroup->format.valueFormat );
		
		// By default we will sort by value
		sort_by_value = true;
		
		// Get the rigth function to compare by value
		compareByValueFunction  = value->getCompareByValueFunction();
		
		//Alloc space for the kv's (known in advance)
		num_kvs =  setGroup->getNumKVs();
		if( num_kvs == 0 )
		{
			kvs = NULL;
			return;
		}
			
		kvs = (KV**) malloc( sizeof(KV*) * num_kvs );
		assert( kvs );
		size_t counter = 0;
		
			
		for (int set_pos = 0 ; set_pos < (int)setGroup->size() ; set_pos++)
		{
			KVSet *set = setGroup->at(set_pos);
			assert( set->isOnMemory() );

			
			KVVector *kvVector = set->newKVVector();
			kvVectors.push_back( kvVector );
			
			// Get the pointers to the all the KV elements
			for (size_t i = 0 ; i < kvVector->num_kvs ; i++)
				kvs[counter++] = &kvVector->kvs[i];
		
		}
		assert(counter == num_kvs);			
		

	}	
	
	KVReader::~KVReader()
	{
		if( kvs )
			free( kvs );
		
		for (size_t i = 0 ; i < kvVectors.size() ; i++)
			delete kvVectors[i];
		
	}
	
	
	/**
	 New collections of functions to sort
	 */
	
	void KVReader::sort_by_key()
	{
		
		// Global sort by key (only grouping)
		std::sort( kvs , kvs+num_kvs , compareByKey ); 
		
	}
	
	
	size_t KVReader::getNumKVS_WithSameKey( size_t index )
	{
		//std::cout << "Pos : " << pos << std::endl;
		
		size_t num = 1;
		while( ((index+num) < num_kvs) && ( compareFullByKey( kvs[index] , kvs[index+num] ) == 0 ) )
			num++;
		
		if(compareByValueFunction && sort_by_value && ( num > 1 ) )
		{
			// sort by key
			std::sort( kvs + index , kvs + index + num , compareByValueFunction);
		}

		return num;
	}
}



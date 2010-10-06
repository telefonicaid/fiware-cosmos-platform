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
#include "samson.h"
#include "KVQueue.h"
#include "Lock.h"                /* Lock                                     */
#include "KVReader.h"            /* Own interface                            */



#pragma mark Auxiliar functions to get the bounds of a part of the vector
#pragma mark -


namespace ss {

	#pragma mark Key Sort Functions
	
	
	inline int compareFullByKey(KV*kv1 , KV*kv2)
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
				  
	


#pragma mark KVMultipleReader
	
	KVMultipleReader::KVMultipleReader( std::vector<KVSetVector*> setsGroups )
	{
		assert( setsGroups.size() < 100 );
		assert( setsGroups.size() > 0 );
		
		// All the elements are suppoused to be the same type
		
		num = vectors.size();
		
		for (size_t i = 0 ; i < setsGroups.size() ; i++)
		{
			vectors.push_back( new KVReader( setsGroups[i] ) );
			index[i] = 0;
			finish[i] = (vectors[i]->num_kvs == 0);
		}
		
		
	}

	
	KVSET* KVMultipleReader::getNextReduceSets()
	{
/*		
		std::cout << "Get next reduce set " << std::endl;
		std::cout << " indexs " << index[0] << "/" << vectors[0]->num_kvs << std::endl;
		std::cout << " indexs " << index[1] << "/" << vectors[1]->num_kvs << std::endl;
*/		
		// check if there are pendign elements
		int pos = 0;
		while( finish[pos] )
			if( ++pos == num)
				return NULL;

		// Put all participants to "0"
		for (int i = 0 ; i < num ; i++)
			participate[i] = false;

		// Set the first as the min key
		int pos_min_key = 0;
		while( finish[pos_min_key] )
			pos_min_key++;
		participate[pos_min_key] = true;
		
		//Find the min key and the participants
		for (int i = pos_min_key ; i < num ; i++)
		{
			if( !finish[i] )	//Still some elements in this 
			{
				int res = compareFullByKey( vectors[pos_min_key]->kvs[index[pos_min_key]] , vectors[i]->kvs[index[i]] );
				
				if ( res == 0)
					participate[i] = true;
				else if ( res < 0 )
					{
						// new min key (previous participants are not longer participants)
						pos_min_key = i;
						participate[i] = true;
						for (int j = 0 ; j < i ; j++)
							participate[j]=false;
					}
			}
		}
		
		
		// Prepare the set ( sorting by value if necessary )
		
		for (int i = 0 ; i < num ; i++)
		{
			if( participate[i] )
			{
				
				size_t num_kvs = vectors[i]->getNumKVS_WithSameKey( index[i] );

				
				sets[i].kvs = vectors[i]->kvs + index[i];
				sets[i].num_kvs = num_kvs;
				
				index[i] += num_kvs;
				
				if( index[i] >= vectors[i]->num_kvs)
					finish[i] = true;
				
				
			}
			else
			{
				sets[i].kvs = NULL;
				sets[i].num_kvs = 0;
			}
		}
		
		return sets;
	}
	
	
}



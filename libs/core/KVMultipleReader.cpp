/* ****************************************************************************
*
* FILE              KVMultipleReader.cpp
*
*  Created by ANDREU URRUELA PLANAS on 4/12/10.
*  Copyright 2010 TID. All rights reserved.
*
*/
#include <assert.h>              /* assert                                   */

#include <samson/KVSetStruct.h>  /* KVSetStruct                              */
#include "KVReaderCompare.h"     /* compareFullByKey, compareByKey           */
#include "KVMultipleReader.h"    /* Own interface                            */



namespace ss {
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

	
	KVSetStruct* KVMultipleReader::getNextReduceSets()
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

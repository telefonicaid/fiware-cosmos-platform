#ifndef SAMSON_KV_MULTIPLE_READER_H
#define SAMSON_KV_MULTIPLE_READER_H

/* ****************************************************************************
*
* KVMultipleReader.h - hold a set of KVSetStructs with common key-type
*
*
*/
#include "KVReader.h"            /* KVReader                                 */
#include "KVSetStruct.h"         /* KVSetStruct                              */



namespace ss
{
    class KVMultipleReader
    {
		std::vector<KVReader*> vectors;

		int           num;                // Number of sets at the input ( max 100 )
		size_t        index[100];         // Index for each input
		bool          participate[100];   // Flag to indicate that this set participate in this round of sets
		bool          finish[100];        // Flag to indicate that this input is finish (no more kvs)
		KVSetStruct   sets[100];          // Used for output

	public:
		KVFormat format;            // Format for the key-value

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

		KVSetStruct* getFullSets()
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
		
		KVSetStruct* getNextReduceSets();
		
		
		
	};
}

#endif

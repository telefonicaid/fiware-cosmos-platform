
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_hit_reduceHits
#define _H_SAMSON_hit_reduceHits


#include <samson/module/samson.h>

#include "samson/modules/hit/HitsState.h"
#include "samson/modules/system/UInt.h"
#include "samson/modules/system/UInt.h"

#include "ContinuousTopList.h"



namespace samson{
namespace hit{


class reduceHits : public samson::Reduce
{


	au::ContinuousTopList top;

	samson::hit::Hit hit;
	samson::hit::HitsState hitsState;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt hit.Hit
	input: system.UInt hit.HitsState
	output: system.UInt hit.HitsState

	helpLine: Reduce to accumulate top entries
#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{

	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		// Parse state
		// ------------------------------------------------------------

		if( inputs[1].num_kvs > 1 )
		{
			OLM_T(LMT_User06, ("Error, not possible inputs[1].num_kvs(%lu) > 1\n", inputs[1].num_kvs));
			// Error, not possible two states
		}
		else if (  inputs[1].num_kvs == 1 )
		{

			// Parse and load state
			hitsState.parse( inputs[1].kvs[0]->value );
			OLM_T(LMT_User06, ("Loading hitsState with hitsState.hits_length:%lu\n", hitsState.hits_length));
			for (int i = 0 ; i < hitsState.hits_length ; i++ )
			{
				top.insert( hitsState.hits[i].time.value ,  hitsState.hits[i].hits.value ,  hitsState.hits[i].concept.value );
			}
		}

		// Parse hits
		// ------------------------------------------------------------
		OLM_T(LMT_User06, ("Parsing hits with inputs[0].num_kvs:%lu\n", inputs[0].num_kvs));
		for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		{
			hit.parse( inputs[0].kvs[i]->value );
			top.insert( hit.time.value, hit.hits.value, hit.concept.value);
		}


	}

	void finish(samson::KVWriter *writer )
	{
		if ((top.top_list.size() > 0) || (top.current_top_list.size() > 0))
		{
			// Emit the top elements as the output state ( key 0 )
			hitsState.hitsSetLength(0);

			// Fill the information
			hitsState.hitsSetLength(0);
			top.fill( &hitsState );

			// Prepare the key
			samson::system::UInt key;
			key.value = 0;

			// Emit state as unique key-value
			OLM_T(LMT_User06, ("Emit hitsState with hitsState.hits_length:%lu\n", hitsState.hits_length));
			writer->emit( 0 , &key , &hitsState );
		}

	}



};


} // end of namespace hit
} // end of namespace samson

#endif

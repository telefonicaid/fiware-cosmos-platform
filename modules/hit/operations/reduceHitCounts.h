
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_hit_reduceHitCounts
#define _H_SAMSON_hit_reduceHitCounts


#include <samson/module/samson.h>

#include <samson/modules/hit/HitCount.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>



namespace samson{
namespace hit{


class reduceHitCounts : public samson::Reduce
{


   samson::system::String key;         // Key concept
   samson::system::UInt tmp_hits;      // Input counter for this key 

	samson::hit::HitCount hitCount;    // State hitCounter for this concept

	unsigned long current_time;        // Current time common to all the key-values

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
/*	
	If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)
	
	input: system.String system.UInt
	input: system.String hit.HitCount

	output: system.UInt hit.HitCount      // emit the output if it changes significantly
	output: system.String hit.HitCount
	
	helpLine: Aggregation of the hits per string
*/

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
		current_time = time(NULL)/300; // Blocks of 5 minute ( to be selected with environment variable )
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		if( inputs[0].num_kvs > 0 )
		{
			key.parse( inputs[0].kvs[0]->key );
			hitCount.parse( inputs[0].kvs[0]->value );
		}
		else if( inputs[1].num_kvs > 0 )
		{
			key.parse( inputs[1].kvs[0]->key );
			hitCount.init( current_time , 0 );  // Current hits '0'
		}
		else
			tracer->setUserError("Running operation with any key-value at input 0 or input 1");


		// Get the number of hits
		size_t num_hits = 0;
		for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		{
			tmp_hits.parse( inputs[0].kvs[i]->value );
			num_hits += tmp_hits.value;
		}

		// Update the hitCount structure with the number of this.
		// It returns true if we need to notify at the output
		bool sent_update = hitCount.update( current_time , num_hits );


		// Emit state at output "0" if necessary ( changes significantly )
		if( sent_update )
		   writer->emit( 0,  &key , &hitCount );

		// Emit the state at the output if still we have some counts....
		if( hitCount.hasContent() )
			writer->emit( 1 , &key , &hitCount );

	}

	void finish(samson::KVWriter *writer )
	{
		OLM_T(LMT_User06, ("reduceHitCounts::finish()\n"));
	}



};


} // end of namespace hit
} // end of namespace samson

#endif

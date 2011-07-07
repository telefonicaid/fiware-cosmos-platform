
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_string_reduceHitCounts
#define _H_SAMSON_string_reduceHitCounts


#include <samson/module/samson.h>

#include <samson/modules/system/HitCount.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>

#include <time.h>

namespace samson{
namespace string{


	class reduceHitCounts : public samson::Reduce
	{

	public:

	  samson::system::String key;
	  samson::system::UInt tmp_hits;
	  samson::system::HitCount hitCount;
	  size_t num_hits;
	  unsigned long current_time;


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.String system.Void  
input: system.String system.HitCount  
output: system.String system.HitCount

helpLine: Aggregation of the hits per string
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		  current_time = time(NULL)%60;// Blocks of one minute ( to be selected with environment variable)
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		  if( inputs[0].num_kvs > 0)
		    key.parse( inputs[0].kvs[0]->key );
	          else if( inputs[1].num_kvs > 0)
		    key.parse( inputs[0].kvs[0]->key );
		  else
		    tracer->setUserError("Running operation with any key-value at input 0 or input 1");

		  // Get the number of hits
		  num_hits = 0;
		  for( size_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		  {
		    tmp_hits.parse( inputs[0].kvs[i]->value );
		    num_hits += tmp_hits.value;
		  }

		  if( inputs[1].num_kvs == 0 )
		    hitCount.init( current_time , num_hits );
		  else if( inputs[1].num_kvs == 1)
		  {
		    hitCount.parse( inputs[1].kvs[0]->value );
		    hitCount.update( current_time , num_hits );
		  }
		  else
		  {
		    char line[1024];
		    sprintf(line , "There are more than one state information for this key: '%s'" , key.value.c_str());
		    tracer->setUserError( line );
		  }

		  // Emit the state at the output if still we have state
		  if( hitCount.hasContent() )
		    writer->emit( 1 , &key , &hitCount );



		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace samson
} // end of namespace string

#endif

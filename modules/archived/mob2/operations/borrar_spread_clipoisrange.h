
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_borrar_spread_clipoisrange
#define _H_SAMSON_mob2_borrar_spread_clipoisrange


#include <samson/module/samson.h>
#include <samson/modules/mob2/ItinRange.h>
#include <samson/modules/mob2/Itinerary.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace mob2{


	class borrar_spread_clipoisrange : public samson::Map
	{
		//Inputs
		samson::system::UInt node;
		Itinerary itin;
		//Outputs
		ItinRange output;
		samson::system::UInt range;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt mob2.Itinerary  
output: mob2.ItinRange system.UInt

extendedHelp: 		Spread itineraries by client, movement and range

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			for(uint64_t i=0; i<inputs[0].num_kvs; i++)
			{
				node.parse(inputs[0].kvs[i]->key);
				itin.parse(inputs[0].kvs[i]->value);
				output.node.value = node.value;
				output.poiSrc.value = itin.source.value;
				output.poiTgt.value = itin.target.value;
				output.group.value = 0;
				output.range.value = 0;
				range.value = itin.range.value;
				writer->emit(0, &output,&range);
			}
		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
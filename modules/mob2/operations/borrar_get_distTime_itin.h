
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_mob2_borrar_get_distTime_itin
#define _H_SAMSON_mob2_borrar_get_distTime_itin


#include <stdint.h>
#include <samson/module/samson.h>
#include <samson/modules/mob2/ItinMovement.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>



namespace samson{
namespace mob2{


class borrar_get_distTime_itin : public samson::Reduce
{
	// Inputs
	ItinMovement move;
	// Outputs
	samson::system::Void mr_void;
	//samson::system::UInt node;
	samson::system::UInt distance;

public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
	// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

	input: system.UInt mob2.ItinMovement
	output: system.UInt system.samson::system::Void

	extendedHelp: 		Extract the number of minutes in movements of a client

#endif // de INFO_COMMENT

	void init(samson::KVWriter *writer )
	{
	}

	void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		int difmonth, difday, difhour, difmin;
		int nMinsMonth;

		for(uint64_t i=0; i<inputs[0].num_kvs; i++)
		{
			//node.parse(inputs[0].kvs[i]->key);
			move.parse(inputs[0].kvs[i]->value);
			difmonth = move.target.date.month.value - move.source.date.month.value;
			if(difmonth > 1)
			{
				return;
			}
			difday = move.target.date.day.value - move.source.date.day.value;
			difhour = move.target.time.hour.value - move.source.time.hour.value;
			difmin = move.target.time.minute.value - move.source.time.minute.value;
			if(move.source.date.month.value==4 || move.source.date.month.value==6 || move.source.date.month.value==9 ||
					move.source.date.month.value==11)
			{
				nMinsMonth = 1440 * 30;
			}
			else if(move.source.date.month.value == 2)
			{
				nMinsMonth = 1440 * 28;
			}
			else
			{
				nMinsMonth = 1440 * 31;
			}
			distance.value = (nMinsMonth * difmonth) + (1440 * difday) + (60 * difhour) + difmin;
			writer->emit(0, &distance,&mr_void);
		}

	}

	void finish(samson::KVWriter *writer )
	{
	}



};


} // end of namespace mob2
} // end of namespace samson

#endif

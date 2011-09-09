
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_mob2_vector_spread_nodedayhour
#define _H_SAMSON_mob2_vector_spread_nodedayhour


#include <samson/module/samson.h>
#include <samson/modules/cdr/mobCdr.h>
#include <samson/modules/mob2/Node_Bts.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>
#include <samson/modules/system/DateComplete.h>
#include <samson/modules/system/Time.h>




namespace samson{
namespace mob2{


	class vector_spread_nodedayhour : public samson::Map
	{
        //Inputs
        samson::system::UInt node;
        samson::cdr::mobCdr cdr;
        //Outputs
        Node_Bts noddayhour;
        samson::system::Void mrvoid;
	// Local variables to handle weekday
	samson::system::DateComplete lDate;
	samson::system::Time lTime;


	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt cdr.mobCdr  
output: mob2.Node_Bts system.Void

extendedHelp: 		Spread cdrs by node, day and hour

#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
        for(uint64_t i=0; i<inputs[0].num_kvs; i++)
        {
                node.parse(inputs[0].kvs[i]->key);
                cdr.parse(inputs[0].kvs[i]->value);
                noddayhour.phone.value = node.value;
                noddayhour.bts.value = 0;
		cdr.timeUnix.getDateTimeFromTimeUTC(&lDate, &lTime);
                int date = lDate.week_day.value;
                // Group 0: MON-TUE, Group 1: FRI, Group 2: SAT, Group 3: SUN
                if(date == 0) 	   { noddayhour.wday.value = 3;}
                else if(date == 5) { noddayhour.wday.value = 1;}
                else if(date == 6) { noddayhour.wday.value = 2;}
                else    	   { noddayhour.wday.value = 0;}
                noddayhour.range.value = lTime.hour.value;
                writer->emit(0, &noddayhour,&mrvoid);
        }

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace mob2
} // end of namespace samson

#endif
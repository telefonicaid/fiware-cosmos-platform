
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_OTTstream_red_update_last_user_profile
#define _H_SAMSON_OTTstream_red_update_last_user_profile


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/Activity.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/system/Int32.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/TimeUnix.h>
#include <samson/modules/level/ValTimed.h>

#include <time.h>


namespace samson{
namespace OTTstream{


class red_update_last_user_profile : public samson::Reduce
{

	samson::OTTstream::Activity activity;
	samson::system::UInt userId;
	samson::OTTstream::ServiceHit hit;
	samson::system::Int32 incOne;
	samson::system::Int32 decOne;
	samson::system::Int32 zero;
	samson::level::ValTimed outVal;
	samson::system::TimeUnix oldestHit;
	int timespan;

	samson::system::String serviceStr;

#define MAX_STR_LEN 20
	char line[MAX_STR_LEN];

public:


	//  INFO_MODULE
	// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
	// Please, do not remove this comments, as it will be used to check consistency on module declaration
	//
	//  input: system.UInt OTTstream.ServiceHit
	//  input: system.UInt OTTstream.Activity
	//  output: system.String level.ValTimed
	//  output: system.UInt OTTstream.Activity
	//
	// helpLine: Update the latest user profile
	//  END_INFO_MODULE

	void init( samson::KVWriter *writer )
	{
		timespan = environment->getInt("OTTstream.timespan", 600);                // Activity window length, in seconds

#define INITIAL_FUTURE ((time_t)0xffffffffffffffff)
		oldestHit.value = INITIAL_FUTURE;

		incOne.value = 1;
		decOne.value = -1;
		zero.value = 0;
	}

	void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
	{
		if ( inputs[1].num_kvs == 0 )
		{
			// New user state
			activity.init();
			userId.parse( inputs[0].kvs[0]->key );
		}
		else
		{
			// Previous state
			activity.parse( inputs[1].kvs[0]->value ); // We only consider one state
			userId.parse( inputs[1].kvs[0]->key );
			if (inputs[1].num_kvs > 1)
			{
				LM_W(("Multiple states(%lu) for user:%lu", inputs[1].num_kvs, userId.value));
			}
		}

		if (inputs[0].num_kvs == 0)
		{
			if (oldestHit.value != INITIAL_FUTURE)
			{
				for (int j= 0; (j < activity.servActivity_length); j++)
				{
					serviceStr.value = activity.servActivity[j].serviceId.str();

					//					if ((oldestHit.value > (activity.servActivity[j].timestamp.value + timespan)) && (activity.servActivity[j].inCount.value == 1))
					//					{
					//						writer->emit(0, &serviceStr, decOne);
					//						activity.servActivity[j].inCount.value = 0;
					//					}
				}
				return;
			}
		}

		//LM_M(("For user:%lu, detected:%lu hits", userId.value,  inputs[0].num_kvs ));
		for (uint64_t i = 0 ; i < inputs[0].num_kvs ; i++ )
		{
			bool serviceFound = false;
			hit.parse(inputs[0].kvs[i]->value);

			//LM_M(("Check for userId:%lu serviceId:%lu", userId.value, hit.serviceId.value));
			for (int j= 0; (j < activity.servActivity_length); j++)
			{

				serviceStr.value = activity.servActivity[j].serviceId.str();

				if (hit.timestamp.value < oldestHit.value)
				{
					oldestHit.value = hit.timestamp.value;
				}

				if (hit.serviceId.value == activity.servActivity[j].serviceId.value)
				{
					serviceFound = true;
					if (hit.timestamp.value > (activity.servActivity[j].timestamp.value + timespan))
					{
						if (activity.servActivity[j].inCount.value == 0)
						{
							outVal.val.value = incOne.value;
							outVal.t.value = hit.timestamp.value;
						    writer->emit(0, &serviceStr, &outVal);
							//writer->emit(0, &serviceStr, &incOne);
							//LM_M(("Emit serviceId:'%s', count:%d, userId:%lu, ts:%s, prv:%s",serviceStr.value.c_str(), incOne.value, userId.value, hit.timestamp.str().c_str(), activity.servActivity[j].timestamp.str().c_str()));
							activity.servActivity[j].inCount.value = 1;
						}
						activity.servActivity[j].timestamp.value = hit.timestamp.value;
					}
				}
				else
				{
					if ((hit.timestamp.value > (activity.servActivity[j].timestamp.value + timespan)) && (activity.servActivity[j].inCount.value == 1))
					{
						outVal.val.value = decOne.value;
						outVal.t.value = hit.timestamp.value;
					  writer->emit(0, &serviceStr, &outVal);
						//LM_M(("Emit serviceId:'%s by hit:%lu', count:%d, userId:%lu, ts:%s, prev:%s",serviceStr.value.c_str(), hit.serviceId.value, decOne.value, userId.value,  hit.timestamp.str().c_str(), activity.servActivity[j].timestamp.str().c_str()));
						//writer->emit(0, &serviceStr, &decOne);
						activity.servActivity[j].inCount.value = 0;
					}
				}
//				{
//					writer->emit(0, &serviceStr, &zero);
//				}
			}

			if ((serviceFound == false) && (hit.serviceId.value != 0))
			{
				serviceStr.value = hit.serviceId.str();
				outVal.val.value = incOne.value;
				outVal.t.value = hit.timestamp.value;
				writer->emit(0, &serviceStr, &outVal);
				//LM_M(("Emit first serviceId:'%s', count:%d, userId:%lu",serviceStr.value.c_str(), incOne.value, userId.value ));
				//writer->emit(0, &serviceStr, &incOne);

				//LM_M(("Create serviceId:%lu, count:%d, userId:%lu, at ts:%s(%lu)",hit.serviceId.value, 1, userId.value,  hit.timestamp.str().c_str(), hit.timestamp.value));
				activity.addService(hit.serviceId, hit.timestamp, 1);
			}
		}
		//LM_M(("Update user:%lu state with %d services", userId.value, activity.servActivity_length));
		writer->emit(1, &userId, &activity);
	}

	void finish( samson::KVWriter *writer )
	{
	}

};


} // end of namespace OTTstream
} // end of namespace samson

#endif
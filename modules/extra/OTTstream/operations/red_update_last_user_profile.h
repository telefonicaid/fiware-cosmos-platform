
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_OTTstream_red_update_last_user_profile
#define _H_SAMSON_OTTstream_red_update_last_user_profile


#include <samson/module/samson.h>
#include <samson/modules/OTTstream/LastHistory.h>
#include <samson/modules/OTTstream/ServiceHit.h>
#include <samson/modules/system/Int32.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>


namespace samson{
namespace OTTstream{


	class red_update_last_user_profile : public samson::Reduce
	{

	public:


//  INFO_MODULE
// If interface changes and you do not recreate this file, you will have to update this information (and of course, the module file)
// Please, do not remove this comments, as it will be used to check consistency on module declaration
//
//  input: system.UInt OTTstream.ServiceHit  
//  input: system.UInt OTTstream.LastHistory  
//  output: system.String system.Int32
//  output: system.UInt OTTstream.LastHistory
//  
// helpLine: Update the latest user profile
//  END_INFO_MODULE

		void init( samson::KVWriter *writer )
		{
		}

		void run( samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		}

		void finish( samson::KVWriter *writer )
		{
		}



	};


} // end of namespace OTTstream
} // end of namespace samson

#endif

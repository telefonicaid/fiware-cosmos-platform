/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_map_site_user
#define _H_SAMSON_osn_map_site_user


#include <samson/module/samson.h>
#include <string>



namespace samson{
namespace osn{


	class map_site_user : public samson::Map
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			//samson::system::UInt64 msisdn;
			//samson::osn::URLConnection  connect;
			//samson::system::String key_out;
			//samson::system::UInt value_out;

			std::string user_agent = environment->get("osn.user_agent_query", "iphone");
			std::string domain = environment->get("osn.domain_query", "search.yahoo.com");

			const char *p_user_agent = user_agent.c_str();
			const char *p_domain = domain.c_str();



			for (size_t i=0; (i < inputs[0].num_kvs); i++)
			{
				samson::system::UInt64 msisdn;
				samson::osn::URLConnection  connect;
				samson::system::String key_out;
				samson::system::UInt value_out;

				msisdn.parse(inputs[0].kvs[i]->key);
				connect.parse(inputs[0].kvs[i]->value);

				if ((strcasestr(connect.userAgent.value.c_str(), p_user_agent)) && (strstr(connect.url.url_host.value.c_str(), p_domain)))
				{


					key_out = connect.url.url_host;

					value_out.value = 1;
					writer->emit(0, &key_out, &value_out);
					key_out.value.clear();
				}


			}
			samson::system::UInt key_total;
			samson::system::UInt val_total;
			key_total.value = 1;
			val_total.value = inputs[0].num_kvs;
			writer->emit(1, &key_total, &val_total);
		}
	};


} // end of namespace samson
} // end of namespace osn

#endif

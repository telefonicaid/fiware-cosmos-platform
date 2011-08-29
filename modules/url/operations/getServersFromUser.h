
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_getServersFromUser
#define _H_SAMSON_url_getServersFromUser


#include <samson/module/samson.h>




namespace samson{
namespace url{


	class getServersFromUser : public samson::Reduce
	{
		samson::system::UInt user;
		samson::system::UInt serverId;

		ServerCount serverCount;


	public:

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{

			bool first = true;
			size_t serverIdPrev = 0;
			int count = 0;
			ServerCountVector servers;

			if (inputs[0].num_kvs == 0)
			{
				return;
			}

			//OLM_T(LMT_User06,("Starts with inputs[0].num_kvs:%d", inputs[0].num_kvs));


			user.parse(inputs[0].kvs[0]->key);
			//OLM_T(LMT_User06, ("Working user:%d with inputs[0].num_kvs:%d", user.value, inputs[0].num_kvs));



			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				serverId.parse(inputs[0].kvs[i]->value);
				if (first || (serverId == serverIdPrev))
				{
					count++;
					first = false;
				}
				else
				{
					serverCount.server = serverIdPrev;
					serverCount.count = count;
					servers.serversCountAdd()->copyFrom(&serverCount);
					count = 1;
				}
				serverIdPrev = serverId.value;
			}
			serverCount.server = serverIdPrev;
			serverCount.count = count;
			servers.serversCountAdd()->copyFrom(&serverCount);

			//OLM_T(LMT_User06,("userId: %d. Complete Vector with %d elements", user.value, servers.serversCount_length));

			for (int i = 0; (i < servers.serversCount_length); i++)
			{
				serverId = servers.serversCount[i].server;
				servers.countRef = servers.serversCount[i].count;

				writer->emit(0, &serverId, &servers);
			}

			//OLM_T(LMT_User06,("userId: %d. Emitted Vector with %d elements", user.value, servers.serversCount_length));

		}


	};


} // end of namespace samson
} // end of namespace url

#endif


/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_export_connections
#define _H_SAMSON_osn_export_connections


#include <samson/Operation.h>


namespace ss{
namespace osn{


	class export_connections : public ss::ParserOut
	{
		ss::system::String key;
		ss::osn::URLConnection connect;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i=0; (i < inputs[0].num_kvs); i++)
			{
				key.parse(inputs[0].kvs[i]->key);
				connect.parse(inputs[0].kvs[i]->value);
				snprintf(output, MAX_STR_LEN, "(%s)\t%lu\t%s\t%d\t%s\t%s\n", key.value.c_str(), connect.msisdn.value, connect.url.completeURL.value.c_str(), connect.status.value, connect.MIMEcontent.value.c_str(), connect.userAgent.value.c_str());
				writer->emit(output);
			}
		}


	};


} // end of namespace ss
} // end of namespace osn

#endif


/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_exportString_UInt
#define _H_SAMSON_url_exportString_UInt


#include <samson/samson.h>


namespace ss{
namespace url{


	class exportString_UInt : public ss::ParserOut
	{

	public:
		ss::system::String key;
		ss::system::UInt val;
#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif


#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				key.parse(inputs[0].kvs[i]->key);
				val.parse(inputs[0].kvs[i]->value);

				snprintf(output, MAX_STR_LEN, "%s|%lu\n", key.value.c_str(), val.value);

				writer->emit(output);
			}
		}


	};


} // end of namespace ss
} // end of namespace url

#endif

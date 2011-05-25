
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_exportUInt_UInt
#define _H_SAMSON_url_exportUInt_UInt


#include <samson/module/samson.h>

/**********************************************************
parserOut exportUInt_UInt
{
	in system.UInt system.UInt
	out txt txt

	helpLine "parserOut for UInt-UInt type data-sets"
}
**********************************************************/

namespace samson{
namespace url{


	class exportUInt_UInt : public samson::ParserOut
	{
		samson::system::UInt key;
		samson::system::UInt val;
#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif


#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];


	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				key.parse(inputs[0].kvs[i]->key);
				val.parse(inputs[0].kvs[i]->value);

				snprintf(output, MAX_STR_LEN, "%lu|%lu\n", key.value, val.value);

				writer->emit(output);
			}
		}


	};


} // end of namespace samson
} // end of namespace url

#endif


/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_export_N_sites
#define _H_SAMSON_osn_export_N_sites


#include <samson/module/samson.h>


namespace samson{
namespace osn{


	class export_N_sites : public samson::ParserOut
	{
		samson::osn::Count count;
		samson::system::Void value;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			for (size_t i=0; (i < inputs[0].num_kvs); i++)
			{
				count.parse(inputs[0].kvs[i]->key);
				value.parse(inputs[0].kvs[i]->value);

				snprintf(output, MAX_STR_LEN, "%lu --> %s\n", count.count.value, count.element.value.c_str());

				writer->emit(output);
			}
		}


	};


} // end of namespace samson
} // end of namespace osn

#endif

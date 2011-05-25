
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_export_total
#define _H_SAMSON_osn_export_total


#include <samson/module/samson.h>



namespace samson{
namespace osn{


	class export_total : public samson::ParserOut
	{
		samson::system::UInt total;
		samson::system::Void value;
#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{

			total.parse(inputs[0].kvs[0]->key);
			//value.parse(inputs[0].kvs[0]->value);


			snprintf(output, MAX_STR_LEN, "%lu\n", total.value);

			writer->emit(output);
		}


	};


} // end of namespace samson
} // end of namespace osn

#endif

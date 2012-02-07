
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sort_export_sequence_common_key
#define _H_SAMSON_sort_export_sequence_common_key


#include <samson/module/samson.h>


namespace samson{
namespace sort{


	class export_sequence_common_key : public samson::ParserOut
	{
		samson::system::UInt8 key;
		samson::system::UInt number;

#ifdef MAX_STR_LEN
#undef MAX_STR_LEN
#endif


#define MAX_STR_LEN 1024
			char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{


			if (inputs[0].num_kvs == 0)
			{
				return;
			}

			//OLM_T(LMT_User01, ("export_sequence_common_key: inputs[0].num_kvs:%ld", inputs[0].num_kvs));

            for (size_t i = 0; (i < inputs[0].num_kvs); ++i)
            {

                    number.parse( inputs[0].kvs[i]->value );
                    //samson::system::UInt8 key0;
                    //key0.parse(inputs[0].kvs[0]->key);
                    //key.parse(inputs[0].kvs[i]->key);
                    //OLM_T(LMT_User01, ("key:%d (key[0]:%d, i:%ld, val:%ld", key.value, key0.value, i, number.value));

                    snprintf(output, MAX_STR_LEN, "%lu\n", number.value);

                    writer->emit(output);
            }
		}


	};


} // end of namespace samson
} // end of namespace sort

#endif
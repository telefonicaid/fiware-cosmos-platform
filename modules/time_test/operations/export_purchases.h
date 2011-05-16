
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_time_test_export_purchases
#define _H_SAMSON_time_test_export_purchases


#include <samson/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>



namespace ss{
namespace time_test{


	class export_purchases : public ss::ParserOut
	{

#define MAX_STR_LEN 1024
		char output[MAX_STR_LEN];

	public:


		void run(KVSetStruct* inputs , TXTWriter *writer )
		{
			char *wdays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
			ss::system::UInt8 day;
			ss::system::UInt total_amount;

			for (size_t i = 0; (i < inputs[0].num_kvs); i++)
			{
				day.parse( inputs[0].kvs[i]->key );
				total_amount.parse( inputs[0].kvs[i]->value );

				snprintf(output, MAX_STR_LEN, "%s --> %lu\n", wdays[day.value], total_amount.value);

				writer->emit(output);
			}
		}


	};


} // end of namespace ss
} // end of namespace time_test

#endif

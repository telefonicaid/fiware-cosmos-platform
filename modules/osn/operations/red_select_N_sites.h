
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_osn_red_select_N_sites
#define _H_SAMSON_osn_red_select_N_sites


#include <samson/module/samson.h>



namespace samson{
namespace osn{


	class red_select_N_sites : public samson::Reduce
	{
		samson::system::UInt key_in;
		samson::osn::Count count;
		samson::system::Void value_out;


	public:

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			int num_sites = environment->getInt ("osn.num_sites", 10);
			std::string total_visits_str = environment->get ("osn.total_count", "1");
			int total_visits = strtol(total_visits_str.c_str(), NULL, 10);





			for (size_t i = (inputs[0].num_kvs-1), j = 0; ((i >= 0) && (j < num_sites)); i--, j++)
			{
				key_in.parse(inputs[0].kvs[i]->key);
				count.parse(inputs[0].kvs[i]->value);


				writer->emit(0, &count, &value_out);
			}

		}
	};


} // end of namespace samson
} // end of namespace osn

#endif

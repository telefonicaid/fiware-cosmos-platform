
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sort_reduce_identity
#define _H_SAMSON_sort_reduce_identity


#include <samson/samson.h>


namespace ss{
namespace sort{


	class reduce_identity : public ss::Reduce
	{
		ss::system::UInt val;
		ss::system::Void dummy;

	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			//OLM_T(LMT_User01, ("reduce_identity: inputs[0].num_kvs:%lu", inputs[0].num_kvs));
			for (int i = 0; (i < inputs[0].num_kvs); ++i)
			{
				val.parse(inputs[0].kvs[i]->key);
				writer->emit(0, &val, &dummy);
			}
		}


	};


} // end of namespace ss
} // end of namespace sort

#endif

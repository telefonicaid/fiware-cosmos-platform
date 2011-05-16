
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_cdr_sna_spread_cdrs_to_links
#define _H_SAMSON_cdr_sna_spread_cdrs_to_links


#include <samson/Operation.h>


namespace ss{
namespace cdr{


	class spread_cdrs_to_links : public ss::Map
	{
		ss::system::UInt number;
		ss::cdr::CDR cdr;
		ss::system::UInt destination;
		
	public:

		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			
			//Simple emit the input node_id - cdr pairs	
			for (size_t i=0  ; i< inputs[0].num_kvs ;i++)
			{
				number.parse(inputs[0].kvs[i]->key);
				cdr.parse(inputs[0].kvs[i]->value);
				
				destination = cdr.node;
				
				//Emit both directions
				writer->emit(0,&number, &destination);
				writer->emit(0,&destination, &number);
			}				
		}
	};


} // end of namespace ss
} // end of namespace cdr

#endif

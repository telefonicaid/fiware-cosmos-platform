
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_convert_dyads_to_comm
#define _H_SAMSON_sna_convert_dyads_to_comm


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class convert_dyads_to_comm : public samson::Map
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			  //input
			  samson::system::UInt id_link;
			  Link link;

			  //output
			  Clique clique_id;
			  Clique clique_out;

			  for (size_t i = 0; i < inputs[0].num_kvs; i++) {
			    id_link.parse(inputs[0].kvs[i]->key );
			    link.parse(inputs[0].kvs[i]->value );

			    clique_id.nodesSetLength(0);
			    clique_out.nodesSetLength(0);

#ifdef nodesAdd
			    clique_id.nodesAdd(id_link);
#else
			    clique_id.nodesAdd()->value = id_link.value;
#endif/* de nodesAdd */

			    clique_id.nodesAdd()->value = link.id.value;

			    writer->emit(0, &clique_id, &clique_out );
			  }
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif
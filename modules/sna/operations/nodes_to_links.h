
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_nodes_to_links
#define _H_SAMSON_sna_nodes_to_links


#include <samson/Operation.h>


namespace ss{
namespace sna{


	class nodes_to_links : public ss::Map
	{

	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			  //Inputs
			  ss::system::UInt node_id;
			  Node node;

			  //Outputs
			  Link_Edges link_key;
			  Link link_value;


			  for (int i=0; i < inputs[0].num_kvs; i++) {

			    node_id.parse(inputs[0].kvs[i]->key);
			    node.parse(inputs[0].kvs[i]->value);

			    for (int j=0; j < node.links_length; j++) {

			      link_key.phoneA = node.id;
			      link_key.phoneB = node.links[j].id;
			      link_value.copyFrom(&node.links[j]);
			      writer->emit(0, &link_key, &link_value);
			    }
			  }
		}


	};


} // end of namespace ss
} // end of namespace sna

#endif

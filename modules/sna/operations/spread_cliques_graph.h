
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_spread_cliques_graph
#define _H_SAMSON_sna_spread_cliques_graph


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class spread_cliques_graph : public samson::Map
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			Clique		input_clique;			//key
			Clique_Node	input_node_clique;		//value

			for (int i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				//Process input of clique cliques
				input_clique.parse( inputs[0].kvs[i]->key );
				input_node_clique.parse( inputs[0].kvs[i]->value );

				//Emir yourself
				writer->emit(0, &input_clique, &input_node_clique);

				//Spread cliques information to compute cliques of cliques
				for (int c = 0 ; c < input_node_clique.links_length ; c++)
					writer->emit(0, &input_node_clique.links[c].clique, &input_node_clique);
			}

		}


	};


} // end of namespace samson
} // end of namespace sna

#endif

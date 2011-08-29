
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_get_possible_cliques_links
#define _H_SAMSON_sna_get_possible_cliques_links


#include <samson/module/samson.h>
#include "sna_environment_parameters.h"


namespace samson{
namespace sna{


	class get_possible_cliques_links : public samson::Reduce
	{

	public:

		bool fusing_communities ;

		void init(samson::KVWriter *writer)
		{
			fusing_communities = environment->getInt(SNA_FLAG_FUSING_COMMUNITIES, 0);
		}


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			Clique cliques[1000] ;
			samson::system::UInt nodeId ;

			if ( inputs[0].num_kvs < 2)	//If there is no cross between two cliques with this node... nothing to do
				return;

			//Limitation in this implementation
			if (inputs[0].num_kvs > 1000 )
			{
				OLM_E(("Error: (inputs[0].num_kvs(%d) > 1000 ), Limitation in this implementation", inputs[0].num_kvs));
				exit(1);
			}

			// Parse the nodeId
			nodeId.parse(inputs[0].kvs[0]->key);

			// Parse all cliques connected througth this node
			for (size_t i = 0  ; i < inputs[0].num_kvs ; i++)
				cliques[i].parse( inputs[0].kvs[i]->value );

			for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
				for (size_t j = i+1 ; j < inputs[0].num_kvs ; j++)
				{
					// We will only generate output for thouse connection where I am the samller common element
					if (cliques[i].fistElementInCommonWithOtherClique(&cliques[j]) == nodeId.value )
					{

						// When fussing communities, we only consider connections between communities with 2 elements in common
						if( !fusing_communities || (cliques[i].numberOfNodesInCommon( &cliques[j] ) >= 2))
						{

							//Get a list of all the nodes
							std::vector<uint> nodes_to_send;
							for (int n = 0 ; n < cliques[i].nodes_length ; n++)
								nodes_to_send.push_back(cliques[i].nodes[n].value);
							for (int n = 0 ; n < cliques[j].nodes_length ; n++)
								nodes_to_send.push_back(cliques[j].nodes[n].value);

							// Sort and remove duplicates
							std::sort( nodes_to_send.begin() , nodes_to_send.end() );
							nodes_to_send.erase(std::unique(nodes_to_send.begin(), nodes_to_send.end()), nodes_to_send.end());

							//Prepare the Clique-link
							Clique2 clique_link;
							clique_link.clique_1.copyFrom(&cliques[i]);
							clique_link.clique_2.copyFrom(&cliques[j]);


							// Emit for all the nodes in the combination
							for (uint n = 0 ;  n < nodes_to_send.size() ; n++)
							{
								samson::system::UInt nodeId;
								nodeId.value = nodes_to_send[n];

								writer->emit(0, &nodeId, &clique_link );
							}

						}
					}
				}
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif

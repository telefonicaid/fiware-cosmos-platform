
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_get_unnecessary_cliques
#define _H_SAMSON_sna_get_unnecessary_cliques


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class get_unnecessary_cliques : public samson::Reduce
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
#define MAX_COMMUNITIES_PER_NODE	10000

			samson::system::UInt nodeId ;
			Clique cliques[MAX_COMMUNITIES_PER_NODE] ;
			samson::system::Void void_data ;

			if( inputs[0].num_kvs > MAX_COMMUNITIES_PER_NODE)
			{
				nodeId.parse(inputs[0].kvs[0]->key);
				OLM_E(("Error: Node: %d with communities %d > MAX_COMMUNITIES_PER_NODE(%d)", nodeId.value, inputs[0].num_kvs, MAX_COMMUNITIES_PER_NODE));
				return;
			}

			if ( inputs[0].num_kvs >= MAX_COMMUNITIES_PER_NODE )
			{
				OLM_E(("Error: ( inputs[0].num_kvs(%d) >= MAX_COMMUNITIES_PER_NODE(%d) )", inputs[0].num_kvs, MAX_COMMUNITIES_PER_NODE))
			}

			for (int i = 0  ; i < inputs[0].num_kvs ; i++)
				cliques[i].parse( inputs[0].kvs[i]->value );

			//Compare all possible combinations

			for (int i = 0 ; i < inputs[0].num_kvs ; i++)
			{
				for (int j = i+1 ; j < inputs[0].num_kvs ; j++)
				{
					// Clique already emited
					if(cliques[j].nodes_length == 0)
					{
						continue;
					}

					// Comparing i with j
					if( cliques[i].nodes_length < cliques[j].nodes_length)
					{
						//Emit an output only if the shortest is included in the longest
						if (cliques[i].isContained(&cliques[j]))
						{
							writer->emit(0, &cliques[i],&void_data);
							break;
						}
					}
					else if( cliques[i].nodes_length > cliques[j].nodes_length)
					{
						//Emit an output only if the shortest is included in the longest
						if (cliques[j].isContained(&cliques[i]))
						{
							writer->emit(0, &cliques[j],&void_data);
							cliques[j].nodesSetLength(0);
						}
					}
					// If both cliques/communities has the same length, it is not necessary to check if one contains the otehr
				}
			}
		}


	};


} // end of namespace samson
} // end of namespace sna

#endif

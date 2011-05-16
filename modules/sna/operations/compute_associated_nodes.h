
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_compute_associated_nodes
#define _H_SAMSON_sna_compute_associated_nodes


#include <samson/Operation.h>
#include "sna_environment_parameters.h"



namespace ss{
namespace sna{


	class compute_associated_nodes : public ss::Reduce
	{

	public:

		double threshold_to_add_associated;

		void init(){
			threshold_to_add_associated = environment->getDouble (SNA_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES, SNA_PARAMETER_THRESHOLD_TO_ADD_ASSOCIATED_NODES_DEFAULT);
		}

		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			ss::sna::Clique clique;
			ss::sna::Vector_Node nodes;

			ss::sna::Clique associated_nodes;

			if( inputs[0].num_kvs <= 0)
				return;

			//Parse everything
			clique.parse( inputs[0].kvs[0]->key );

			nodes.itemsSetLength(0);
			for (int i = 0 ; i < inputs[0].num_kvs ; i++)
				nodes.itemsAdd()->parse( inputs[0].kvs[i]->value );

			//Compute associated members
			nodes.computeAssociatedNodes( &associated_nodes  , threshold_to_add_associated  );

			writer->emit(0, &clique, &associated_nodes);
		}


	};


} // end of namespace ss
} // end of namespace sna

#endif

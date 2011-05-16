
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_acg_red_cdrs_to_nodes
#define _H_SAMSON_sna_acg_red_cdrs_to_nodes


#include <samson/Operation.h>
#include "sna_environment_parameters.h"


namespace ss{
namespace sna{


	class acg_red_cdrs_to_nodes : public ss::Reduce
	{
		int num_cdrs_for_strong;

	public:
		void init ()
		{
			OLM_E(("acg_red_cdrs_to_nodes::init()"));

			num_cdrs_for_strong = environment->getInt(SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION, SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION_DEFAULT);
		}


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			ss::sna::Node node;

			//Just compact information...
			ss::system::UInt mobile; // Current destination mobile we are currently counting
			int counter;		// Number of times we have this number

			int counter_dir0;
			int sum_dur;		// Number of times we have this number
			int sum_dur_dir0;	// Number of times we have this number

			if (inputs[0].num_kvs == 0)
				return;

			ss::system::UInt node_id;	// Reference node
			ss::sna::CDR cdr;		
			ss::system::UInt destination;	// Destination of the cdr

			// Already in init()
			//num_cdrs_for_strong = environment->getInt(SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION, SNA_PARAMETER_CDRS_TO_BE_STRONG_CONNECTION_DEFAULT);


			node_id.parse( inputs[0].kvs[0]->key); //Get the number of the current mobile
			node.id.value = node_id.value;

			//OLM_T(LMT_User01, ("node_id:%ld, Sets node_id:%ld, node.id:%ld", node_id.value, node_id.value, node.id.value));


			cdr.parse(inputs[0].kvs[0]->value); 
			//destination.parse( inputs[0].kvs[0]->value );    //Parse the first kv "value"

			//Take the current number and put counter to 1
			mobile.value = cdr.node.value ;

			counter = 1;
			sum_dur = cdr.duration.value ;
			counter_dir0 = 0 ;
			sum_dur_dir0 = 0 ;

			if ( cdr.dir.value == 0 )
			{
				++counter_dir0;
				sum_dur_dir0 += cdr.duration.value ;
			}

			node.linksSetLength(0);

			//OLM_T ( LMT_User01, ("----------------------"));
			//OLM_T ( LMT_User01, ("Leyendo telefono: %ld", node.id.value));
			//OLM_T ( LMT_User01, ("Leyendo telefono: %ld contacto: %d th: %d", node.id.value, cdr.node, num_cdrs_for_strong));

			for (int i=1 ; i < inputs[0].num_kvs ; ++i) {
				//destination.parse( inputs[0].kvs[i].value );    //Parse the value for a new cdr
				cdr.parse( inputs[0].kvs[i]->value); //Parse the value for a new cdr
				//mobile = cdr.node ;

				if (cdr.node.value == mobile.value ) {
					++counter;
					sum_dur += cdr.duration.value ;

					if (cdr.dir.value == 0 ) {
						++counter_dir0;
						sum_dur_dir0 += cdr.duration.value ;
					}
				} else {
					//Save the previous one
					ss::sna::Link *link = node.linksAdd();
					link->id.value = mobile.value;
					// This ss::sna::Link::double_2_intScaled() function (previuosly sna_double2int()) hidens a 1000 multiplication
					link->weight.value = ss::sna::Link::double_2_intScaled((double)counter / (double)num_cdrs_for_strong  );
					link->dir.value = ss::sna::Link::double_2_intScaled((double)counter_dir0 / (double)counter );

					// Keep the new number and put counter back to one again
					// mobile = destination.value;
					mobile.value = cdr.node.value ;
					counter = 1;
					sum_dur = cdr.duration.value ;
					counter_dir0 = 0 ;
					sum_dur_dir0 = 0 ;

					if ( cdr.dir.value == 0 ){
						++counter_dir0;
						sum_dur_dir0 += cdr.duration.value ;
					}
				}
			}

			//Add the last one...
			ss::sna::Link *link = node.linksAdd();
			link->id.value = mobile.value;
			link->weight.value = ss::sna::Link::double_2_intScaled( (double)counter / (double)num_cdrs_for_strong  );
			link->dir.value    = ss::sna::Link::double_2_intScaled( (double)counter_dir0 / (double)counter );

			//By default all nodes belong to Telefonica
			node.flags.value = ss::sna::Node::NODE_FLAG_NONE;

			//OLM_T(LMT_User01, ("node_id:%ld, Emits node_id:%ld, node.id:%ld", node_id.value, node_id.value, node.id.value));
			//Emit the complete node information
			writer->emit(0, &node_id, &node);
		}
	};


} // end of namespace ss
} // end of namespace sna

#endif

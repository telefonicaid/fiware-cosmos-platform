
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_red_links_add_3graph
#define _H_SAMSON_sna_red_links_add_3graph


#include <samson/Operation.h>
#include <samson/Log.h>


namespace ss{
namespace sna{


class red_links_add_3graph : public ss::Reduce
{

public:


	void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
	{


		//Inputs
		Link_Edges link_key;
		Link link_value;

#define NUM_INPUTS 3 // In MACRO, the number of inputs was received as an argument, But as there was also an assert(num_inputs == 6), we can assume this

		uint w[NUM_INPUTS];
		uint d[NUM_INPUTS];

		double w_out;
		double d_out;

		//Outputs
		ss::system::UInt id;
		Link link_out;


		for (int i=0 ; i < NUM_INPUTS ; i++){
			w[i] = 0;
			d[i] = 0;
		}

		for (int i=0 ; i < NUM_INPUTS ; i++){

			if ( inputs[i].num_kvs > 0){

				link_key.parse(inputs[i].kvs[0]->key);
				link_value.parse(inputs[i].kvs[0]->value);

				w[i] = link_value.weight.value;
				d[i] = link_value.dir.value;
			}
			else
			{
				OLM_E(("data-set:%d empty ", i));
			}

		}

		w_out = w[0] + w[1] + w[2] ;
		if (w_out <= 0)
		{
			OLM_E(("Error, w_out(%.2f) <= 0", w_out));
			return;
		}

		d_out = (d[0]*w[0] + d[1]*w[1] + d[2]*w[2] )/w_out ;

		id = link_key.phoneA;
		link_out.id = link_key.phoneB;

		link_out.weight.value = (uint) roundl(w_out);
		link_out.dir.value = (uint) roundl(d_out);

		writer->emit(0, &id, &link_out);

#undef NUM_INPUTS

	}


};


} // end of namespace ss
} // end of namespace sna

#endif

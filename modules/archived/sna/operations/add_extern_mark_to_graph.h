
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sna_add_extern_mark_to_graph
#define _H_SAMSON_sna_add_extern_mark_to_graph


#include <samson/module/samson.h>


namespace samson{
namespace sna{


	class add_extern_mark_to_graph : public samson::Reduce
	{

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			samson::system::UInt id;
			samson::sna::Node node;

			if( inputs[0].num_kvs > 1)
			{
				OLM_E(("Error. inputs[0].num_kvs(%d) > 1", inputs[0].num_kvs));
				exit(1);
			}

			//Since the inclussion of the Jajah format, where there can be several users with the same phone, this assertion is not mandatory anymore
			//assert( inputs[1].num_kvs <= 1);

			if( inputs[0].num_kvs == 1)
			{
				id.parse( inputs[0].kvs[0]->key);
				node.parse( inputs[0].kvs[0]->value);

				if( inputs[1].num_kvs == 0) // If we do not have this element, it is considered extern
				{
					node.flags.value = node.flags.value | samson::sna::Node::NODE_FLAG_EXTERN; //Activate the extern flag
				}
				writer->emit(0, &id, &node);
			}
		}
	};


} // end of namespace samson
} // end of namespace sna

#endif
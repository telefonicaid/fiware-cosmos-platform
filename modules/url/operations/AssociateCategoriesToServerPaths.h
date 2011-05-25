
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_AssociateCategoriesToServerPaths
#define _H_SAMSON_url_AssociateCategoriesToServerPaths


#include <samson/module/samson.h>


/*******************************************************************
reduce AssociateCategoriesToServerPaths
{
	in system.UInt url.CategoryVector	  # Server(id) - CategoryVector
	in system.UInt system.String          # Server(id) - Path
	out system.UInt url.ServerPathCount   # Category(id) - ServerPathCount

	helpLine "Assign categories to every ServerPath, and emits cat-serverPath key-values. The weight goes into the count field"
}
********************************************************************/

namespace samson{
namespace url{


	class AssociateCategoriesToServerPaths : public samson::Reduce
	{

	public:
		samson::system::UInt serverIdRef;
		samson::system::UInt serverId;
		samson::system::String path;
		CategoryVector categories;
		samson::system::UInt catId;
		ServerPathCount serverPath;


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			if (inputs[0].num_kvs == 0)
			{
				return;		// We don't have category information for that server
			}

			if (inputs[0].num_kvs != 1)
			{
				OLM_E(("We should have only one category vector per server: inputs[0].num_kvs:%d", inputs[0].num_kvs));
				return;
			}

			serverIdRef.parse(inputs[0].kvs[0]->key);
			categories.parse(inputs[0].kvs[0]->value);

			size_t num_categories = categories.category_length;

			for (int i = 0; (i < inputs[1].num_kvs); i++)
			{
				path.parse(inputs[1].kvs[i]->value);

				serverPath.server = serverIdRef;
				serverPath.path = path;

				size_t countRef = 1;

				for (int k = 0; (k < num_categories); k++)
				{
					catId.value = k;

					serverPath.weight = categories.category[k].weight;
					writer->emit(0, &catId, &serverPath);
				}
			}
		}


	};


} // end of namespace samson
} // end of namespace url

#endif

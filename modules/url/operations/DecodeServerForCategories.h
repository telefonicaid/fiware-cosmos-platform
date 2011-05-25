
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_DecodeServerForCategories
#define _H_SAMSON_url_DecodeServerForCategories


#include <samson/module/samson.h>

/************************************************************************
reduce DecodeServerForCategories
{
    in system.UInt system.String                # Server(id) - Server(string)
	in system.UInt url.CategoryPath             # Server(id) - CategoryPath
	out system.UInt url.ServerStrPath           # Category(id) - ServerStrPath

	helpLine "Reconstruct serverPath information per category, with server as string"
}
*************************************************************************/

namespace samson{
namespace url{


	class DecodeServerForCategories : public samson::Reduce
	{

		samson::system::UInt serverId;
		CategoryPath catPath;
		samson::system::UInt catId;
		samson::system::String serverStr;
		ServerStrPath serverStrPath;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			if (inputs[0].num_kvs != 1)
			{
				OLM_E(("We should have one and only one occurrence of server in the decode server list. inputs[0].num_kvs: %d", inputs[0].num_kvs));
			}

			serverId.parse(inputs[0].kvs[0]->key);
			serverStr.parse(inputs[0].kvs[0]->value);

			serverStrPath.server = serverStr;

			bool first = true;
			size_t n_items = 0;

			for (int i = 0; (i < inputs[1].num_kvs); i++)
			{
				catPath.parse(inputs[1].kvs[i]->value);

				catId = catPath.category;
				serverStrPath.path = catPath.path;
				writer->emit(0, &catId, &serverStrPath);
			}
		}
	};


} // end of namespace samson
} // end of namespace url

#endif

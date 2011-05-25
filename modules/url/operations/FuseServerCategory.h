
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_FuseServerCategory
#define _H_SAMSON_url_FuseServerCategory


#include <samson/module/samson.h>


/*****************************************************************
reduce FuseServerCategory
{
	in system.UInt url.CategoryVector   # Server(id) - CategoryVector (previous reference distribution)
	in system.UInt url.CategoryVector   # Server(id) - CategoryVector (emitted from server similarity)
	out system.UInt url.CategoryVector  # Server(id) - CategoryVector (if !fixed, mean of category distribution)

	helpLine "Fuse previous knowledge of categories with new samples"
}
*******************************************************************/

namespace samson{
namespace url{


	class FuseServerCategory : public samson::Reduce
	{
		samson::system::UInt serverIdRef;
		CategoryVector categoriesRef;
		samson::system::UInt serverId;
		CategoryVector categories;
		CategoryVector categories_out;

	public:


		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
			bool ref_emitted = false;

			//OLM_T(LMT_User06, ("inputs[0].num_kvs:%ld inputs[1].num_kvs:%ld", inputs[0].num_kvs, inputs[1].num_kvs));

			if (inputs[0].num_kvs == 1)
			{
				categoriesRef.parse(inputs[0].kvs[0]->value);
				if (categoriesRef.fixed.value == 1)
				{
					serverIdRef.parse(inputs[0].kvs[0]->key);
					writer->emit(0, &serverIdRef, &categoriesRef);
					//OLM_T(LMT_User06, ("serverIdRef(%d): Emits the fixed reference categories, and finished", serverIdRef.value));
					return;
				}
			}

			if (inputs[1].num_kvs == 0)
			{
				if (inputs[0].num_kvs == 1)
				{
					serverIdRef.parse(inputs[0].kvs[0]->key);
					categoriesRef.parse(inputs[0].kvs[0]->value);
					writer->emit(0, &serverIdRef, &categoriesRef);
				}

				if (inputs[0].num_kvs == 0)
				{
					OLM_E(("We have found a server not present in the category reference list, and not visited by users"));
				}
				else
				{
					OLM_E(("We have found a server(%d) present only in the category reference list, but not visited by users", serverIdRef.value));

				}
				return;
			}

			serverId.parse(inputs[1].kvs[0]->key);
			categories.parse(inputs[1].kvs[0]->value);
			categories_out.copyFrom(&categories);
			int num_categories = categories.category_length;

			//OLM_T(LMT_User06,("serverId(%d): Starting with %d categories", serverId.value, num_categories));


			size_t *conf_acum = new size_t [num_categories];
			size_t *count_acum = new size_t [num_categories];

			// From fakeInfos, we know that every catId has at least one countServer. If that change
			// we should protect it also here
			for (int k = 0; (k < num_categories); k++)
			{
				conf_acum[k] = categories.category[k].confidence.value;
				count_acum[k] = categories.category[k].countServer.value;
			}


			for (int i = 1; (i < inputs[1].num_kvs); i++)
			{
				categories.parse(inputs[1].kvs[i]->value);

				for (int k = 0; (k < num_categories); k++)
				{
					//OLM_T(LMT_User06,("serverId(%d): Accumulate category:%d", serverId.value, k));

					// Accumulate is also performed directly with the integers
					categories_out.category[k].weight.value += categories.category[k].weight.value;
					conf_acum[k] += categories.category[k].confidence.value;
					count_acum[k] += categories.category[k].countServer.value;
				}

			}

			for (int k = 0; (k < num_categories); k++)
			{
				//OLM_T(LMT_User06,("serverId(%d): Normalizing category:%d weight:%d acum:%d newweight:%d", serverId.value, k, categories_out.category[k].weight.value, count_acum[k], categories_out.category[k].weight.value / count_acum[k]));

				// Accumulate is also performed directly with the integers
				categories_out.category[k].weight.value = categories_out.category[k].weight.value / count_acum[k];
				categories_out.category[k].confidence = conf_acum[k]/inputs[1].num_kvs;
				categories_out.category[k].countServer = count_acum[k];
			}
			writer->emit(0, &serverId, &categories_out);

			delete conf_acum;
			delete count_acum;
		}


	};


} // end of namespace samson
} // end of namespace url

#endif

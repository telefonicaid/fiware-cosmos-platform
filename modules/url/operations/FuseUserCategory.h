
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_url_FuseUserCategory
#define _H_SAMSON_url_FuseUserCategory


#include <samson/samson.h>
#include <samson/Log.h>

/*******************************************************************
reduce FuseUserCategory
{
	in system.UInt url.CategoryVector   # User(id) - CategoryVector (emitted from user servers visits)
	out system.UInt url.CategoryVector  # User(id) - CategoryVector

	helpLine "Fuse distribution of categories for users"
}
********************************************************************/

namespace ss{
namespace url{


	class FuseUserCategory : public ss::Reduce
	{
		ss::system::UInt userId;
		CategoryVector categories;
		CategoryVector categories_out;

	public:


		void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
		{
			if (inputs[0].num_kvs == 0)
			{

				OLM_E(("We have found an user without sites visited"));
				return;
			}

			userId.parse(inputs[0].kvs[0]->key);
			//OLM_T(LMT_User06,("For user:%d, inputs[0].num_kvs:%d", userId.value, inputs[0].num_kvs));

			categories.parse(inputs[0].kvs[0]->value);
			categories_out.copyFrom(&categories);
			int num_categories = categories.category_length;

			size_t *conf_acum = new size_t [num_categories];
			size_t *count_acum = new size_t [num_categories];

			for (int k = 0; (k < num_categories); k++)
			{
				conf_acum[k] = categories.category[k].confidence.value;
				count_acum[k] = categories.category[k].countServer.value;
			}

			for (int i = 1; (i < inputs[0].num_kvs); i++)
			{
				categories.parse(inputs[0].kvs[i]->value);

				for (int k = 0; (k < categories.category_length); k++)
				{
					// Accumulate is also performed directly with the integers
					categories_out.category[k].weight.value += categories.category[k].weight.value;
					conf_acum[k] += categories.category[k].confidence.value;
					count_acum[k] += categories.category[k].countServer.value;
					//OLM_T(LMT_User06,("Accumulating for user:%d, cat:%d, weight:%d", userId.value, k, categories_out.category[k].weight.value));
				}
			}

			for (int k = 0; (k < categories_out.category_length); k++)
			{
				// Accumulate is also performed directly with the integers
				categories_out.category[k].weight.value = categories_out.category[k].weight.value / count_acum[k];
				categories_out.category[k].confidence = conf_acum[k]/inputs[0].num_kvs;
				categories_out.category[k].countUser = count_acum[k];
				//OLM_T(LMT_User06,("Normalizing for user:%d, cat:%d, weight:%d, countUser:%d", userId.value, k, categories_out.category[k].weight.value, categories_out.category[k].countUser.value));

			}
			writer->emit(0, &userId, &categories_out);

			delete conf_acum;
			delete count_acum;
		}


	};


} // end of namespace ss
} // end of namespace url

#endif


/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
 */

#ifndef _H_SAMSON_txt_collocation_count_stripes
#define _H_SAMSON_txt_collocation_count_stripes


#include <samson/Operation.h>
#include <samson/modules/system/String.h>
#include <samson/modules/system/UInt.h>
#include "samson/modules/txt/CountData.h"
#include "samson/modules/txt/Stripe.h"
#include <string>
#include <vector>

namespace ss{
namespace txt{

class collocation_count_stripes : public ss::Reduce
{
	ss::system::String key;
	ss::system::String keyPrev;
	ss::txt::Stripe valueIn;

public:


	void run(  ss::KVSetStruct* inputs , ss::KVWriter *writer )
	{
		std::vector<ss::txt::CountData>valueOutTmp;
		ss::txt::Stripe valueOut;

		if( inputs[0].num_kvs == 0 )
		{
			return;
		}

		key.parse(inputs[0].kvs[0]->key);

		valueOutTmp.clear();
		for (size_t i = 0; i < inputs[0].num_kvs ; i++)
		{
			valueIn.parse(inputs[0].kvs[i]->value);

			OLM_T(LMT_User01, ("Treating '%s' with %d collocations", key.value.c_str(), valueIn.colList_length));


			//valueOut.colListSetLength(valueOut.colList_length + valueIn.colList_length);
			OLM_T(LMT_User01, ("Checks %d collocs in valueOutTmp (size:%d)", valueIn.colList_length, valueOutTmp.size()));
			for (size_t j = 0; (j < valueIn.colList_length); j++)
			{
				OLM_T(LMT_User01, ("Checks valueOutTmp for '%s' [%d]", valueIn.colList[j].word.value.c_str(), j));
				bool encontrado = false;
				for (size_t k = 0; (!encontrado && (k < valueOutTmp.size())); k++)
				{
					if (valueIn.colList[j].word.value.compare(valueOutTmp[k].word.value) == 0)
					{
						OLM_T(LMT_User01, ("Increment count"));
						valueOutTmp[k].count += valueIn.colList[j].count;
						encontrado = true;
					}
				}
				if (!encontrado)
				{
					OLM_T(LMT_User01, ("Adds '%s' to valueOutTmp", valueIn.colList[j].word.value.c_str()));
					valueOutTmp.push_back(valueIn.colList[j]);
					OLM_T(LMT_User01, ("New size: %d", valueOutTmp.size()));
				}
			}
		}

		for (size_t k = 0; (k < valueOutTmp.size()); k++)
		{
			if (valueOutTmp[k].count > 0)
			{
				valueOut.colListAdd()->copyFrom(&(valueOutTmp[k]));
			}
		}
		OLM_T(LMT_User01, ("Emit last key: '%s' with %d collocs (%d tmp)", key.value.c_str(), valueOut.colList_length, valueOutTmp.size()));
		writer->emit( 0, &key , &valueOut );
	}
};


} // end of namespace ss
} // end of namespace txt

#endif

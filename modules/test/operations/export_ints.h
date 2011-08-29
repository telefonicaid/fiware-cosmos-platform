
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_test_export_ints
#define _H_SAMSON_test_export_ints


#include <samson/module/samson.h>
#include <samson/modules/test/Integer.h>
#include <sstream>

namespace samson{
namespace test{


	class export_ints : public samson::ParserOut
	{

	public:

		samson::test::Integer key, value;

		void run(KVSetStruct* inputs , TXTWriter *writer )
		{

		  for (size_t i = 0 ; i < inputs->num_kvs ; i++ )
		    {
		      key.parse( inputs[0].kvs[i]->key );
		      value.parse( inputs[0].kvs[i]->value );
		      
		      std::ostringstream o;
		      o << key.value << " " << value.value << "\n";		      
		      writer->emit( o.str() );
		      
		    }


		}


	};


} // end of namespace samson
} // end of namespace test

#endif


/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_tienda_test_generator
#define _H_SAMSON_tienda_test_generator


#include <samson/module/samson.h>


namespace samson{
namespace tienda{


	class test_generator : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
                        size_t num_samples = environment->getInt( "tienda.samples", 10000);

                        samson::system::UInt id;
                        samson::tienda::Operation operation;

                        for (size_t i = 0; (i < num_samples); i++)
                        {
                                id = i;
                                operation.user = rand()%100;
                                operation.product = rand()%10000;
                                operation.operation = rand()%2;

                                writer->emit(0, &id, &operation);
                        }
		}


	};


} // end of namespace samson
} // end of namespace tienda

#endif

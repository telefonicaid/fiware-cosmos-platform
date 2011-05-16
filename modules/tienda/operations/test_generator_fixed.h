
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_tienda_test_generator_fixed
#define _H_SAMSON_tienda_test_generator_fixed


#include <samson/samson.h>


namespace ss{
namespace tienda{


	class test_generator_fixed : public ss::Generator
	{

	public:


		void run( ss::KVWriter *writer )
		{
            size_t num_samples = environment->getInt( "tienda.samples", 10000);

            ss::system::UInt id;
            ss::tienda::Operation operation;

            for (size_t i = 0; (i < num_samples); i++)
            {
                    id = i;
                    operation.user = i%100;
                    operation.product = i%1000;
                    operation.operation = i%2;

                    writer->emit(0, &id, &operation);
            }
		}


	};


} // end of namespace ss
} // end of namespace tienda

#endif

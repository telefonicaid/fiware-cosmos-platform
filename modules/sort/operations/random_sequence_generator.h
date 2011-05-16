
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_sort_random_sequence_generator
#define _H_SAMSON_sort_random_sequence_generator


#include <samson/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/UInt8.h>
#include <samson/modules/system/Void.h>

/**************************************************************************
generator random_sequence_generator
{
	out system.UInt system.Void

	helpLine "Generates a random sequence of UInt-Void key-values, as many as selected with the environment variable 'test.number_of_samples'"
}
***************************************************************************/

namespace ss{
namespace sort{


	class random_sequence_generator : public ss::Generator
	{

	public:


		void run( ss::KVWriter *writer )
		{
            size_t num_samples = environment->getSizeT( "test.number_of_samples", 1000000000);

            ss::system::UInt val1;
            ss::system::UInt val2;
            ss::system::Void dummy;

            for (size_t i = 0; (i < num_samples); i++)
            {
                    val1.value = rand();
                    val2.value = val1.value << 31 + rand();

                    writer->emit(0, &val2, &dummy);
            }
		}


	};


} // end of namespace ss
} // end of namespace sort

#endif

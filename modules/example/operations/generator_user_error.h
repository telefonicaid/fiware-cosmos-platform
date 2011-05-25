
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_example_generator_user_error
#define _H_SAMSON_example_generator_user_error


#include <samson/module/samson.h>

namespace samson{
namespace example{


	class generator_user_error : public samson::Generator
	{

	public:


		void run( samson::KVWriter *writer )
		{
		  // Recover the number of samples from the environment variables                                                                                                                                  
		  size_t num_samples = environment->getSizeT( "example.samples" ,  0 );


		  if( num_samples == 0)
		  {

		    // Example of how to report an error from this point
		    tracer->setUserError("Something horrible happened inside this funciont ;) .... example.samples is not defined or is 0");
		  }
		  else
		  {
		    OLM_M(("Ok!"));
		  }

		}


	};


} // end of namespace samson
} // end of namespace example

#endif

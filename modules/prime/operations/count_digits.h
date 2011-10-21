
/**
 File autogenerated with samsonModuleParser. Please, edit to complete this operation
*/

#ifndef _H_SAMSON_prime_count_digits
#define _H_SAMSON_prime_count_digits


#include <samson/module/samson.h>
#include <samson/modules/system/UInt.h>
#include <samson/modules/system/Void.h>


namespace samson{
namespace prime{


	class count_digits : public samson::Reduce
	{

	   samson::system::UInt key;
	   samson::system::UInt count;

	public:


#ifdef INFO_COMMENT //Just to include a comment without conflicting anything
// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)

input: system.UInt system.Void  
output: system.UInt system.UInt

helpLine: Group how many times a particular digit appears
#endif // de INFO_COMMENT

		void init(samson::KVWriter *writer )
		{
		}

		void run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )
		{
		   key.parse( inputs[0].kvs[0]->key );
		   count.value = inputs[0].num_kvs;


		   writer->emit( 0 , &key , &count);

		}

		void finish(samson::KVWriter *writer )
		{
		}



	};


} // end of namespace prime
} // end of namespace samson

#endif
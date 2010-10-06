

#include "Module.h"
#include <samson/modules/base/Module.h>

void ss::example::generator::run( std::vector<ss::KVWriter *>& writers )
{
	ss::base::UInt a,b;
	
	for (size_t i = 0 ; i < 10000000  ; i++)
	{
		a.value = i;
		b.value = i+1;
		writers[0]->emit(&a, &b);
	}
}



/**

Module: Example module to play with samson
Operation: map
Inputs: base.UInt-base.UInt 
Outputs: base.UInt-base.UInt 

*/


void ss::example::map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	ss::base::UInt key,value;
	
	for ( size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		key.parse( inputs[0].kvs[i]->key );
		value.parse( inputs[0].kvs[i]->value );
		
		outputs[0]->emit(&value,&key);
	
	}
}

/**

Module: Example module to play with samson
Operation: r
Inputs: base.UInt-base.UInt2 
Outputs: base.UInt-system.Void 

*/


void ss::example::r::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	// Your code goes here
}


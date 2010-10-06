
#include "Module.h"



/**

Module: Base Module for the SAMSON platform
Operation: example_map
Inputs: base.UInt-base.UInt 
Outputs: base.UInt-base.UInt 

*/


void ss::base::example_map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	// Your code goes here
	
	UInt key;
	UInt value;
	
	for (size_t i = 0 ; i < inputs[0].num_kvs ; i++)
	{
		key.parse( inputs[0].kvs[i]->key );
		value.parse( inputs[0].kvs[i]->value );
		
		outputs[0]->emit( &value , &key );
		
	}
	
}


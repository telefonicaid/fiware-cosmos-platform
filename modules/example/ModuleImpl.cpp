#include "Module.h"
#include <iostream>

/**

Module: Example module to play with samson
Operation: generator
Inputs: 
Outputs: system.UInt-system.UInt 

*/


void ss::example::generator::run( ss::KVWriter *writer )
{
	ss::system::UInt a;
	ss::system::UInt b;
	
	size_t num_kvs = 1000000;
	
	std::cout << "Running example with "<< num_kvs << std::endl ;
	
	for (size_t i = 0 ; i < num_kvs ; i++)
	{
		a = (i%10);
		b = 2*(i%10);
		writer->emit( 0 , &a , &b );
	}
	
}

/**

Module: Example module to play with samson
Operation: map
Inputs: system.UInt-system.UInt 
Outputs: system.UInt-system.UInt 

*/


void ss::example::map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	//Write here your code
}


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
	
	std::cout << "Running example\n";
	
	for (int i = 0 ; i < 10 ; i++)
	{
		a = i;
		b = i*i;
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


#include "Module.h"


/**

Module: Example module to play with samson
Operation: generator
Inputs: 
Outputs: base.UInt-base.UInt 

*/


void ss::example::generator::run( std::vector<ss::KVWriter *>& writers )
{
	//Write here yout code
}

/**

Module: Example module to play with samson
Operation: map
Inputs: base.UInt-base.UInt 
Outputs: base.UInt-base.UInt 

*/


void ss::example::map::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	//Write here yout code
}

/**

Module: Example module to play with samson
Operation: r
Inputs: base.UInt-base.UInt2 
Outputs: base.UInt-system.Void 

*/


void ss::example::r::run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )
{
	//Write here yout code
}


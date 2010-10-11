

#include <iostream>
#include "ModulesManager.h"
#include "samson/Data.h"

int main( int argc , char *argv[] )
{
	std::cout << "Test\n";
	
	ss::ModulesManager mm;

	
	ss::Data *data = mm.getData("example.example");
	
	if( data )
		std::cout << data->help();
}
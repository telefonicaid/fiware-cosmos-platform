
#include "samson/common/EnvironmentOperations.h"			// Own definition of this methods

#include <iostream>
#include <iomanip>


namespace samson {

	void copyEnviroment( Environment* from , gpb::Environment * to )
	{
		std::map<std::string,std::string>::iterator iter;
		for ( iter = from->environment.begin() ; iter != from->environment.end() ; iter++)
		{
			gpb::EnvironmentVariable *ev = to->add_variable();			
			
			ev->set_name( iter->first );
			ev->set_value( iter->second );
		}
		
	}

	void copyEnviroment( const gpb::Environment & from , Environment* to  )
	{
		for( int i = 0 ; i < from.variable_size() ; i++ )
		{
			std::string name = from.variable(i).name();
			std::string value = from.variable(i).value();
			to->set( name , value );
		}
	}

    
    
}
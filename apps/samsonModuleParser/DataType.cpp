

/* ****************************************************************************
 *
 * FILE                     DataType.cpp
 *
 * DESCRIPTION				Definition of a basic data-type
 *
 * ***************************************************************************/


#include "DataType.h"		// Own interface


namespace ss {

	std::vector<std::string> tockenizeWithDots( std::string myString )
	{
		assert( myString.length() < 1000 );
		char tmp[1000];
		strcpy(tmp, myString.c_str() );
		std::vector<std::string> tockens;
		char *p = strtok(tmp, ".");
		while (p) {
			tockens.push_back( std::string(p) ); 
			p = strtok(NULL, " ");
		}			
		return tockens;
	}


	std::string getModuleFromFullName( std::string fullName )
	{
		std::vector<std::string> tockens = tockenizeWithDots( fullName );
		ostringstream output;
		
		for (size_t i = 0 ; i < ( tockens.size()-1) ; i++)
		{
			output << tockens[i];
			if ( i < ( tockens.size()-2) )
				output << ".";
		}
		return output.str();
	}

	std::string getNameFromFullName( std::string fullName )
	{
		std::vector<std::string> tockens = tockenizeWithDots( fullName );
		return tockens[ tockens.size()-1 ];
	}

}
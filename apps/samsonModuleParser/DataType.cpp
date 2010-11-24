/* ****************************************************************************
*
* FILE                     DataType.cpp
*
* DESCRIPTION				Definition of a basic data-type
*
*/
#include <string.h>         // strcpy, strtok
#include "DataType.h"		// Own interface



namespace ss
{
	std::vector<std::string> tockenizeWithDots(std::string myString)
	{
		assert(myString.length() < 1000);
		char tmp[1000];

		strcpy(tmp, myString.c_str());
		std::vector<std::string> tokens;

		char *p = strtok(tmp, ".");
		while (p)
		{
			tokens.push_back(std::string(p));
			p = strtok(NULL, " ");
		}			

		return tokens;
	}


	std::string getModuleFromFullName(std::string fullName)
	{
		std::vector<std::string>  tokens = tockenizeWithDots(fullName);
		ostringstream             output;
		
		for (size_t i = 0; i < (tokens.size() - 1); i++)
		{
			output << tokens[i];
			if (i < (tokens.size() - 2))
				output << ".";
		}

		return output.str();
	}

	std::string getNameFromFullName( std::string fullName )
	{
		std::vector<std::string> tokens = tockenizeWithDots(fullName);
		return tokens[tokens.size() - 1];
	}

}

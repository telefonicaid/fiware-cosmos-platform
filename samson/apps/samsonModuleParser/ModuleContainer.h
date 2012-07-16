#ifndef MODULE_CONTAINER_H
#define MODULE_CONTAINER_H

/* ****************************************************************************
*
* FILE                     ModuleContainer.h
*
* DESCRIPTION				Information about a module (module definition)
*
*/

#include <stdio.h>
#include <unistd.h>             /* _exit, ...                              */
#include <string.h>             /* strtok, strcpy, ...                     */
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>

#include <samson/module/KVFormat.h>
#include "DataType.h"

namespace samson
{
	class DataCreator;
	class AUTockenizer;
	
	class ModuleContainer
	{
		
	public:
		
		std::string name;    // Short name for this module
		std::string title;   // Long title for better description
        
		std::string author;
		std::string version;
		
		std::vector <std::string> includes;
		std::vector <std::string> help;
		
		std::map<std::string,std::string> sets;
		
        // Constructor
		ModuleContainer( );

		/** Get a unique name for this module for the _H_ define */
		
		std::string getDefineUniqueName();
		
		std::string getClassName();
		
		std::string getFullClassName();
		
		std::vector<std::string> tockenizeWithDots( std::string myString );
		
		void parse( AUTockenizer *module_creator ,int begin ,int end );
		
	};

}

#endif

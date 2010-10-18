#include <stdio.h>
#include <iostream>				// std::cerr

#include "samsonDirectories.h"	// SAMSON_SETUP_FILE
#include "CommandLine.h"		// au::CommandLine
#include "SamsonSetup.h"		// Own interface

namespace ss
{
	static SamsonSetup *samsonSetup = NULL;
	
	SamsonSetup *SamsonSetup::shared()
	{
		if( !samsonSetup)
			samsonSetup = new SamsonSetup();
		return samsonSetup;
	}
	
	SamsonSetup::SamsonSetup()
	{
		setDefaultValues();
		
		FILE *file = fopen( SAMSON_SETUP_FILE  ,"r");
		if (!file)
		{
			std::cerr << "Config file "<< SAMSON_SETUP_FILE <<" not found\n";
			return;
		}

		char line[2000];
		while( fgets(line, sizeof(line), file))
		{
			au::CommandLine c;
			c.parse(line);
			
			if( c.get_num_arguments() == 0 )
				continue;
			
			// Skip comments
			std::string mainCommand = c.get_argument(0);
			if( mainCommand[0] == '#' )
				continue;
			
			if (c.get_num_arguments() >= 2)	
			{
				set( c.get_argument(0) ,c.get_argument(1) );
			}

		}
		
		

	
	
	}
	
	
}

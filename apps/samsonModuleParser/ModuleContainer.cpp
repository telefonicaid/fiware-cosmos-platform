
/* ****************************************************************************
 *
 * FILE                     ModuleContainer.cpp
 *
 * DESCRIPTION				Information about a module (module definition)
 *
 * ***************************************************************************/

#include "ModuleContainer.h"
#include "DataContainer.h"
#include "DataCreator.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"			// AUTockenizer

namespace ss {
	
	void ModuleContainer::parse( AUTockenizer *module_creator ,  int begin ,int end )
	{
		
		int pos = begin;
		
		while( pos < end )
		{
			assert( !module_creator->isSpecial( pos ) );
			std::string mainCommand = module_creator->itemAtPos( pos++ );
			
			if( mainCommand == "title" )
				title = module_creator->getLiteral(&pos);
			else if( mainCommand == "author" )
				author = module_creator->getLiteral(&pos);
			else if( mainCommand == "version" )
				version = module_creator->getLiteral(&pos);
			else if( mainCommand == "include" )
			{
				std::string fileName = module_creator->itemAtPos(pos++);
				includes.push_back( fileName );
			}
			else if( mainCommand == "set" )
			{
				std::string setName		= module_creator->itemAtPos(pos++);
				std::string setValue	= module_creator->itemAtPos(pos++);
				sets.insert( std::pair<std::string,std::string>( setName , setValue) );
			}
			else if( mainCommand == "help" )
			{
				int begin,end;
				module_creator->getScopeLimits(&pos, &begin,&end);
				while( begin < end)
					help.push_back( module_creator->getLiteral(&begin) );
				
			}
			else
			{
				fprintf(stderr, "Error: Unknown command inside module section (%s)\n", mainCommand.c_str());
				exit(0);
			}
			
			
		}
		
		assert( pos == (end+1));	//Make sure parsing is correct
		
	}
}

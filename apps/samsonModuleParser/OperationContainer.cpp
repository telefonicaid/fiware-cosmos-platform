/* ****************************************************************************
 *
 * FILE                     OperationContainer.cpp
 *
 * DESCRIPTION				Information about an operation  (module definition)
 *
 * ***************************************************************************/

#include "DataContainer.h"
#include "DataCreator.h"
#include "OperationContainer.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"					// AUTockenizer

namespace ss {
	
	void OperationContainer::parse( AUTockenizer *module_creator ,  int begin ,int end )
	{
		int pos = begin;
		while( pos < end )
		{
			assert( !module_creator->isSpecial( pos ) );
			std::string mainCommand = module_creator->itemAtPos( pos++ );
			
			if( mainCommand == "file" )
				file = module_creator->getLiteral(&pos);
			else if( mainCommand == "code" )
				code = module_creator->getLiteral(&pos);
			else if( mainCommand == "helpLine" )
				helpLine = module_creator->getLiteral(&pos);
			else if( mainCommand == "setup" )
				setup = true;
			else if( mainCommand == "top" )
				top = true;
			else if( mainCommand == "destructor" )
				destructor = true;
			else if( mainCommand == "dynamic_input_formats" )
				dynamic_input_formats = true;
			else if( mainCommand == "extern" )
			{
				//Extern command...
				
				ostringstream o;
				while ( !module_creator->isSemiColom( pos ) )
					o << module_creator->itemAtPos( pos++ ) << " ";
				o << ";";
				pos++;
				
				functions.push_back( o.str() );
				
			}
			else if( mainCommand == "help" )
			{
				int begin,end;
				module_creator->getScopeLimits(&pos, &begin,&end);
				while( begin < end)
					help.push_back( module_creator->getLiteral(&begin) );
				
			}
			else if( mainCommand == "in" )
			{
				
				assert( !module_creator->isSpecial( pos ) );
				std::string keyFormat = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				std::string valueFormat = module_creator->itemAtPos( pos++ );		
				
				inputs.push_back( KVFormat::format(keyFormat, valueFormat) );
				
			}
			else if( mainCommand == "out" )
			{
				
				assert( !module_creator->isSpecial( pos ) );
				std::string keyFormat = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				std::string valueFormat = module_creator->itemAtPos( pos++ );		
				
				outputs.push_back( KVFormat::format(keyFormat, valueFormat) );
				
			}
			else
			{
				fprintf(stderr, "Error: Unknown command inside  opertion section (%s)\n", mainCommand.c_str());
				exit(0);
			}
			
			
		}
		
		assert( pos == (end+1));	//Make sure parsing is correct
		
	}
}

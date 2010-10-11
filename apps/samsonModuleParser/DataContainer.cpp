
/* ****************************************************************************
 *
 * FILE                     DataContainer.cpp
 *
 * DESCRIPTION				Information about a data type (module definition)
 *
 * ***************************************************************************/


#include "DataContainer.h"
#include "DataCreator.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"	// AUTockenizer

namespace ss {

	bool DataContainer::parse( AUTockenizer *module_creator ,  int begin , int end )
	{

		int pos = begin;
		
		while( pos < end )
		{
			assert( !module_creator->isSpecial( pos ) );
			std::string mainCommand = module_creator->itemAtPos( pos++ );
		
			if( mainCommand == "file" )
				file = module_creator->getLiteral(&pos);
			else if( mainCommand == "vector" )
			{
				//Vector fiels
				
				DataType data_type;
				data_type.vector = true;
				
				assert( !module_creator->isSpecial( pos ) );
				data_type.type = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				data_type.name = module_creator->itemAtPos( pos++ );
				
				assert( module_creator->isSemiColom( pos ) );
				pos++;
				
				addItem( data_type );
				
			}
			else
			{
				//Normal fiels
				DataType data_type;
				data_type.vector = false;
				data_type.type = mainCommand;

				assert( !module_creator->isSpecial( pos ) );
				data_type.name = module_creator->itemAtPos( pos++ );

				if( !module_creator->isSemiColom( pos ) )
				{
					fprintf(stderr, "Error parsing document at Data %s (%s %s) \n", name.c_str() ,  data_type.type.c_str() , data_type.name.c_str());
					exit(0);
				}
				pos++;
				
				addItem( data_type );
				
			}
			
		}
		
		assert( pos == (end+1));	//Make sure parsing is correct
									  

	//How to parse an element like this
	return true;

	}
}


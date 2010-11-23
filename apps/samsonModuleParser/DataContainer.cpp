
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
		
			if( mainCommand == "vector" )
			{
				//Vector fiels
				
				
				assert( !module_creator->isSpecial( pos ) );
				std::string _full_type = module_creator->itemAtPos( pos++ );
				
				assert( !module_creator->isSpecial( pos ) );
				std::string _name = module_creator->itemAtPos( pos++ );
				
				assert( module_creator->isSemiColom( pos ) );
				pos++;
				
				
				DataType data_type( _full_type , _name , true);
				addItem( data_type );
				
			}
			else
			{
				//Normal fiels
				std::string  _full_type = mainCommand;

				assert( !module_creator->isSpecial( pos ) );
				std::string _name = module_creator->itemAtPos( pos++ );

				if( !module_creator->isSemiColom( pos ) )
				{
					fprintf(stderr, "Error parsing document at Data %s (%s %s) \n", name.c_str() ,  _full_type.c_str() , _name.c_str());
					exit(0);
				}
				pos++;
				
				DataType data_type( _full_type , _name , false);
				addItem( data_type );
				
			}
			
		}
		
		assert( pos == (end+1));	//Make sure parsing is correct
									  

	//How to parse an element like this
	return true;

	}
}


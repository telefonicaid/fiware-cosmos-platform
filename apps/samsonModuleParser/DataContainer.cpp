
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
#include "logMsg.h"					 // LM_M()

namespace ss {

	
	
	bool DataContainer::parse( AUTockenizer *module_creator ,  int begin , int end )
	{

		int pos = begin;
		
		while( pos < end )
		{
			if( module_creator->isSpecial( pos ) )
				LM_X(1,("Error parsing data-type definition"));
			
			std::string mainCommand = module_creator->itemAtPos( pos++ );
		
			if( mainCommand == "vector" )
			{
				//Vector fiels
				
				
				if( module_creator->isSpecial( pos ) )
					LM_X(1,("Error parsing data-type definition"));

				std::string _full_type = module_creator->itemAtPos( pos++ );
				
				if( module_creator->isSpecial( pos ) )
					LM_X(1,("Error parsing data-type definition"));

				std::string _name = module_creator->itemAtPos( pos++ );
				
				if( !module_creator->isSemiColom( pos ) )
					LM_X(1,("Error parsing data-type definition"));

				pos++;
				
				
				DataType data_type( _full_type , _name , true);
				addItem( data_type );
				
			}
			else
			{
				//Normal fiels
				std::string  _full_type = mainCommand;

				if( module_creator->isSpecial( pos ) )
					LM_X(1,("Error parsing data-type definition."));

				std::string _name = module_creator->itemAtPos( pos++ );

				if( !module_creator->isSemiColom( pos ) )
				{
					fprintf(stderr, "Error parsing document at Data %s (%s %s) \n", name.c_str() ,  _full_type.c_str() , _name.c_str());
					exit(1);
				}
				pos++;
				
				DataType data_type( _full_type , _name , false);
				addItem( data_type );
				
			}
			
		}
		
		if( pos != (end+1))
			LM_X(1,("Error parsing data-type definition. Number of items is not correct at the end of parsing"));

									  

	//How to parse an element like this
	return true;

	}
}


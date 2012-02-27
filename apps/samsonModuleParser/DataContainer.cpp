
/* ****************************************************************************
 *
 * FILE                     DataContainer.cpp
 *
 * DESCRIPTION				Information about a data type (module definition)
 *
 * ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>					// exit()
#include "DataContainer.h"
#include "DataCreator.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"	// AUTockenizer

namespace samson
{



bool DataContainer::parse( AUTockenizer *module_creator ,  int begin , int end )
{

	int pos = begin;
	bool optionalData = false;
	bool firstFieldInData = true;
	size_t valMask = 1;

	while( pos < end )
	{
		if( module_creator->isSpecial( pos ) )
		{
			fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
			exit (1);
		}

		std::string mainCommand = module_creator->itemAtPos(pos++).str;

		optionalData = false;
		if (mainCommand == "optional")
		{
			if (firstFieldInData)
			{
				fprintf(stderr, "samsonModuleParser: Error, no optional field allowed if first in data structure (line:%d).\a\a\a\n", module_creator->items[pos].line);
				exit (1);
			}
			else
			{
				optionalData = true;
			}
			mainCommand = module_creator->itemAtPos(pos++).str;
		}

		if( mainCommand == "vector" )
		{
			//Vector fields


			if( module_creator->isSpecial(pos) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}

			std::string _full_type = module_creator->itemAtPos(pos++).str;

			if( module_creator->isSpecial( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}

			std::string _name = module_creator->itemAtPos(pos++).str;

			if( !module_creator->isSemiColom( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}

			pos++;


			DataType data_type( _full_type , _name , DataType::container_vector, optionalData, valMask, module_creator->items[pos].line);
			addItem( data_type);
		}
        else if( mainCommand == "list" )
		{
			//list fields
            
			if( module_creator->isSpecial(pos) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}
            
			std::string _full_type = module_creator->itemAtPos(pos++).str;
            
			if( module_creator->isSpecial( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}
            
			std::string _name = module_creator->itemAtPos(pos++).str;
            
			if( !module_creator->isSemiColom( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}
            
			pos++;
            
            
			DataType data_type( _full_type , _name , DataType::container_list, optionalData, valMask, module_creator->items[pos].line);
			addItem( data_type );
		}
		else
		{
			//Normal fields
			std::string  _full_type = mainCommand;

			if( module_creator->isSpecial( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing data-type definition at line:%d", module_creator->items[pos].line);
				exit (1);
			}

			std::string _name = module_creator->itemAtPos(pos++).str;

			if( !module_creator->isSemiColom( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing document at Data %s (%s %s), line:%d\n", name.c_str() ,  _full_type.c_str() , _name.c_str(), module_creator->items[pos].line);
				exit(1);
			}
			pos++;

			DataType data_type( _full_type , _name , DataType::container_none, optionalData, valMask, module_creator->items[pos].line);
			addItem( data_type);
		}
		firstFieldInData = false;
		if (optionalData)
		{
			valMask <<= 1;
		}
	}

	if( pos != (end+1))
	{
		fprintf(stderr, "samsonModuleParser: Error parsing data-type definition. Number of items is not correct at the end of parsing, ends at line:%d", module_creator->items[pos].line);
		exit (1);
	}

    // Check everything is correct
    if (check() == false)
    {
        fprintf(stderr, "samsonModuleParser: Error checking an data-type definition, close to line:%d\n", module_creator->items[pos].line);
        exit (1);
    }

	//How to parse an element like this
	return true;

}
}


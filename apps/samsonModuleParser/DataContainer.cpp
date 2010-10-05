


#include "DataContainer.h"
#include "DataCreator.h"
#include <cstdio>
#include <map>

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


	bool DataContainer::parse( AUTockenizer *module_creator ,  int begin , int end )
	{

		int pos = begin;
		
		while( pos < end )
		{
			assert( !module_creator->isSpecial( pos ) );
			std::string mainCommand = module_creator->itemAtPos( pos++ );
		
			if( mainCommand == "extern" )
			{
				//Extern command...
			
				ostringstream o;
				while ( !module_creator->isSemiColom( pos ) )
					o << module_creator->itemAtPos( pos++ ) << " ";
				o << ";";
				pos++;
				
				functions.push_back( o.str() );
				
			} else  if( mainCommand == "vector" )
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


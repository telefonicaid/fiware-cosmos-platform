/* ****************************************************************************
 *
 * FILE                     OperationContainer.cpp
 *
 * DESCRIPTION				Information about an operation  (module definition)
 *
 * ***************************************************************************/

#include <stdio.h>
#include "DataContainer.h"
#include "DataCreator.h"
#include "OperationContainer.h"
#include <cstdio>
#include <map>
#include "AUTockenizer.h"					// AUTockenizer

namespace samson {

	std::string OperationContainer::getCompareFunctionForData( std::string data, std::string compareFunction )
	{
		std::vector<std::string> data_names = tockenizeWithDots( data );
		
		std::ostringstream output;
		output << "::samson::";
		for (size_t i = 0 ; i < data_names.size() ;i++)
			output << data_names[i] << "::";
		output << compareFunction;
		return output.str();
		
	}

	std::string OperationContainer::getIncludeForData( std::string data )
	{
		std::vector<std::string> data_names = tockenizeWithDots( data );
		
		std::ostringstream output;
		output << "#include <samson/modules/";
		for (size_t i = 0 ; i < ( data_names.size()-1) ;i++)
			output << data_names[i] << "/";
		output << data_names[data_names.size()-1] << ".h>\n";
		return output.str();
		
	}
	
	
	
	void OperationContainer::parse( AUTockenizer *module_creator ,  int begin ,int end )
	{
		int pos = begin;
		while( pos < end )
		{
			if( module_creator->isSpecial( pos ) )
			{
				fprintf(stderr, "samsonModuleParser: Error parsing an operation definition at line:%d\n", module_creator->items[pos].line);
				exit (1);
			}

			std::string mainCommand = module_creator->itemAtPos(pos++).str;

			if( mainCommand == "file" )
				file = module_creator->getLiteral(&pos);
			else if( mainCommand == "code" )
				code = module_creator->getBlockwithBlanks(&pos);
			else if( mainCommand == "helpLine" )
			{
				helpLine = module_creator->getLiteral(&pos);
				info += "\n";
				info += "helpLine: " + helpLine;
			}
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
					o << module_creator->itemAtPos(pos++).str << " ";
				o << ";";
				pos++;
				
				functions.push_back( o.str() );
				
			}
			else if( mainCommand == "help" )
			{
				int begin,end;
				module_creator->getScopeLimits(&pos, &begin,&end);
				info += "\n";
				info += "extendedHelp: ";
				while( begin < end)
				{
					std::string info_line = module_creator->getLiteral(&begin);
					help.push_back(info_line);
					info+= "\t\t" + info_line + "\n";
				}
				
			}
			else if( mainCommand == "in" )
			{
				
				if( module_creator->isSpecial( pos ) )
				{
					fprintf(stderr, "samsonModuleParser: Error parsing an operation definition. Wrong format at line:%d\n", module_creator->items[pos].line);
					exit (1);
				}

				std::string keyFormat = module_creator->itemAtPos(pos++).str;
				
				if( module_creator->isSpecial( pos ) )
				{
					fprintf(stderr, "samsonModuleParser: Error parsing an operation definition. Wrong format at line:%d\n", module_creator->items[pos].line);
					exit (1);
				}

				std::string valueFormat = module_creator->itemAtPos(pos++).str;
				
				std::string optional_compareKeyFunction = module_creator->itemAtPos(pos++).str;
				std::string compareKeyFunction;

#define COMPAREKEYFUNCTION "compareKeyFunction="
#define COMPAREVALUEFUNCTION "compareValueFunction="

				if (optional_compareKeyFunction.compare(0,strlen(COMPAREKEYFUNCTION),COMPAREKEYFUNCTION) == 0)
				{
					if ((type != "reduce") && (type != "parserOutReduce"))
					{
						fprintf(stderr, "samsonModuleParser: Error, compareKeyFunction available only on inputs to reduce or parserOutReduce operations, not to type:'%s'. Error at name:'%s', line:%d\n", type.c_str(), name.c_str(), module_creator->items[pos-1].line);
						exit (1);
					}
					size_t pos_equal = optional_compareKeyFunction.find("=");
					pos_equal++;
					compareKeyFunction = optional_compareKeyFunction.substr(pos_equal);
				}
				else
				{
					pos--;
					optional_compareKeyFunction = "";
					compareKeyFunction = "";
				}



				std::string optional_compareValueFunction = module_creator->itemAtPos(pos++).str;
				std::string compareValueFunction;
				
				if (optional_compareValueFunction.compare(0,strlen(COMPAREVALUEFUNCTION),COMPAREVALUEFUNCTION) == 0)
				{
					if ((type != "reduce") && (type != "parserOutReduce"))
					{
						fprintf(stderr, "samsonModuleParser: Error, compareValueFunction available only on inputs to reduce or parserOutReduce operations, not to type:'%s'. Error at name:'%s', line:%d\n", type.c_str(), name.c_str(), module_creator->items[pos-1].line);
						exit (1);
					}
					size_t pos_equal = optional_compareValueFunction.find("=");
					pos_equal++;
					compareValueFunction = optional_compareValueFunction.substr(pos_equal);
				}
				else
				{
					pos--;
					optional_compareValueFunction = "";
					compareValueFunction = "";
				}

				info += "input: " + keyFormat + " " + valueFormat + " " + optional_compareKeyFunction + " " + optional_compareValueFunction + "\n";

				DataSet dataset(KVFormat::format(keyFormat, valueFormat), compareKeyFunction, compareValueFunction);
                addInput(dataset);
			}
			else if( mainCommand == "out" )
			{
				
				if( module_creator->isSpecial( pos ) )
				{
					fprintf(stderr, "samsonModuleParser: Error parsing an operation definition type:'%s', name:'%s'. Wrong format at line:%d\n", type.c_str(), name.c_str(),  module_creator->items[pos].line);
					exit (1);
				}

				std::string keyFormat = module_creator->itemAtPos(pos++).str;
				
				if( module_creator->isSpecial( pos ) )
				{
					fprintf(stderr, "samsonModuleParser: Error parsing an operation definition type:'%s', name:'%s'. Wrong format at line:%d\n", type.c_str(), name.c_str(),  module_creator->items[pos].line);
					exit (1);
				}

				std::string valueFormat = module_creator->itemAtPos(pos++).str;
				
				info += "output: " + keyFormat + " " + valueFormat + "\n";
				DataSet dataset(KVFormat::format(keyFormat, valueFormat));
                addOutput(dataset);
			}
			else
			{
				fprintf(stderr, "samsonModuleParser: Error: Unknown command inside  operation section (%s), type:'%s', name:'%s', at line:%d\n", mainCommand.c_str(), type.c_str(), name.c_str(), module_creator->items[pos].line);
				exit(1);
			}
		}
		
		if( pos != (end+1))
		{
			fprintf(stderr, "samsonModuleParser: Error parsing an operation definition. Invalid number of items, at line:%d\n", module_creator->items[pos].line);
			exit (1);
		}
	
        // Check everything is correct
        check();
	}
}

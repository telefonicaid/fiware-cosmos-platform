/* ****************************************************************************
 *
 * FILE                     OperationContainer.h
 *
 * DESCRIPTION				Information about an operation  (module definition)
 *
 * ***************************************************************************/

#pragma once

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
	


	/** 
	 * Container to hold information of a dataset
	 */
	class DataSet
	{
	public:
		KVFormat key_values;
		std::string compareKeyFunction;
		std::string compareValueFunction;

		DataSet (KVFormat _key_values){ key_values = _key_values, compareKeyFunction = "compare"; compareValueFunction = "compare";}
		DataSet (KVFormat _key_values, std::string _compareKeyFunction, std::string _compareValueFunction)
		{
			key_values = _key_values;
			if (_compareKeyFunction  == "")
			{
				compareKeyFunction = "compare";
			}
			else
			{
				compareKeyFunction = _compareKeyFunction;
			}
			if (_compareValueFunction  == "")
			{
				compareValueFunction = "compare";
			}
			else
			{
				compareValueFunction = _compareValueFunction;
			}
		}

	};

	/**
	 Container to hold information for an operation
	 */
	
	class OperationContainer
	{
		
	public:
		
		bool setup;
		
		std::string module;		// Name of the module
		std::string name;		// Name of the operation
		
		
		std::string type;
		std::vector <DataSet> inputs;
		std::vector <DataSet> outputs;
		
		std::string helpLine;				// Help in one line
		std::vector <std::string> help;		// More extended help
		
		std::string info;

		std::string code;		// Code for scripts
		std::string file;		// File where this operation is defined
		std::vector <std::string> functions; 
		
		bool top;
		bool dynamic_input_formats;
		bool destructor;
		
		OperationContainer( std::string _module , std::string _type, std::string _name  )
		{
			module = _module;
			type = _type;
			name = _name; 
			
			setup = false;
			top =  false;
			dynamic_input_formats = false;
			destructor = false;
		}
		
        void addInput( DataSet input_dataset )
        {
            if( type == "parser" )
            {
                std::cerr << "Error in operation " << name << ": Not possible to add inputs to a parser. The unique txt-txt input will be added automatically\n";
                exit(1);
            }
            
            if( type == "map" && inputs.size() > 0)
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Maps can only contain one input\n";
                exit(1);
            }
                
            inputs.push_back(input_dataset);
        }
        
        void addOutput( DataSet output_dataset )
        {
            
            printf("Adding output %s ( %s )\n", name.c_str() , type.c_str() );
            
            if( ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
            {
                std::cerr << "samsonModuleParser: Error in operation " << name << ": Not possible to add outputs to a parserOut or parserOutReduce. The unique txt-txt output will be added automatically\n";
                exit(1);
            }
            
            outputs.push_back(output_dataset);
        }
        
        void check()
        {
            // Check everything is correct
            if ( ( type == "map" ) || ( type == "reduce" ) || ( type == "parser" ) )
                if (outputs.size() == 0)
                {
                    std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation needs an output\n";
                    exit(1);
                }
            
            if ( ( type == "map" ) || ( type == "reduce" ) || ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
                if (inputs.size() == 0)
                {
                    std::cerr << "samsonModuleParser: Error in operation " << name << ": Operation needs an input\n";
                    exit(1);
                }
            
            if ((type == "reduce") || (type == "parserOutreduce"))
            {
            	std::string prevKeyFormat = inputs[0].key_values.keyFormat;

    			for (size_t i = 1 ; i < inputs.size() ; i++)
    			{
    				if (prevKeyFormat != inputs[i].key_values.keyFormat)
    				{
    					fprintf(stderr, "samsonModuleParser: Error in operation '%s' ('%s' type). keyFormat must agree for all inputs ('%s' != '%s')\n", name.c_str(), type.c_str(), prevKeyFormat.c_str(), inputs[i].key_values.keyFormat.c_str());
    					exit(1);
    				}
    			}

               	std::string prevKeyCompareFunction = inputs[0].compareKeyFunction;

				for (size_t i = 1 ; i < inputs.size() ; i++)
				{
					if (prevKeyCompareFunction != inputs[i].compareKeyFunction)
					{
						fprintf(stderr, "samsonModuleParser: Error in operation '%s' ('%s' type). compareKeyFunction must agree for all inputs ('%s' != '%s')\n", name.c_str(), type.c_str(), prevKeyCompareFunction.c_str(), inputs[i].compareKeyFunction.c_str());
						exit(1);
					}
				}
            }

        }
        
		void parse( AUTockenizer *module_creator , int begin , int end );
		
		std::string parentClass()
		{
			if( type == "generator")
				return "samson::Generator";
			
			if( type == "map")
				return "samson::Map";
			
			if( type == "reduce")
				return "samson::Reduce";

			if( type == "script")
				return "samson::Script";

			if( type == "parser")
				return "samson::Parser";
			
			if( type == "parserOut")
				return "samson::ParserOut";

			if( type == "parserOutReduce")
				return "samson::ParserOutReduce";
			
            
			fprintf(stderr, "samsonModuleParser: Error: Unknown type of operation in the operation section (%s)\n" , type.c_str());
			_exit(1);
			
		}
		
		void printFile( std::string directory )
		{
			std::ostringstream fileName;
			fileName << directory;	// base directory
			fileName << "operations/";
			mkdir( fileName.str().c_str() , 0755 );
			fileName << name << ".h";

			std::ifstream _file( fileName.str().c_str() );
			if( _file.is_open() )
			{
				_file.close();
				std::cout << "File " << name << ".h is not generated because it already exist\n";
				return;
			}

			std::cout << "Creating file " << name << ".h" << std::endl;
			
			std::ofstream file( fileName.str().c_str() );
			
			file << "\n";
			file << "/**\n";
			file << " File autogenerated with samsonModuleParser. Please, edit to complete this operation\n";
			file << "*/\n";
			file << "\n";
			
			file << "#ifndef _H_SAMSON_" << module << "_" << name << "\n";
			file << "#define _H_SAMSON_" << module << "_" << name << "\n";
			
			file << "\n";
			file << "\n";
			
			file << "#include <samson/module/samson.h>\n";
			
			file << "\n";
			file << "\n";
			
			// Name space
			file << "namespace samson{\n";
			file << "namespace " << module << "{\n";
			
			file << "\n";
			file << "\n";
			
			// Class definition
			file << "\tclass " << name << " : public " << parentClass() << "\n";
			file << "\t{\n";

			file << "\n";
			
			// Public tag
			file << "\tpublic:\n";
			file << "\n\n";
			
			file << "#ifdef INFO_COMMENT //Just to include a comment without conflicting anything\n";
			file << "// If interface changes and you do not recreate this file, consider updating this information (and of course, the module file)" << "\n";
			file << "\n";
			file << info << "\n";
			file << "#endif // de INFO_COMMENT" << "\n";
			file << "\n";


			if( type != "script")
			{
				
				if( (type == "map") || (type == "reduce") )
					file << "\t\tvoid run(  samson::KVSetStruct* inputs , samson::KVWriter *writer )\n\t\t{\n\t\t}\n";
				if( type == "generator" )
					file << "\t\tvoid run( samson::KVWriter *writer )\n\t\t{\n\t\t}\n";
				if( type == "parser" )
					file << "\t\tvoid run( char *data , size_t length , samson::KVWriter *writer )\n\t\t{\n\t\t}\n";
				if( ( type == "parserOut" ) || ( type == "parserOutReduce" ) )
					file << "\t\tvoid run(KVSetStruct* inputs , TXTWriter *writer )\n\t\t{\n\t\t}\n";

			}
			
			file << "\n\n";
			
			// End of the class
			file <<  "\t};\n";			
			
			
			file << "\n";
			file << "\n";
			
			// End of namspace
			file << "} // end of namespace samson\n";
			file << "} // end of namespace " << module << "\n";
			
			file << "\n";
			
			file << "#endif\n";
			
			
			file.close();			
			
		}
		
		std::string getCompareFunctionForData( std::string data , std::string compareFunction);
		std::string getIncludeForData( std::string data );

		void getIncludes( std::set<std::string>& includes )
		{
			for (size_t i = 0 ; i < inputs.size() ; i++)
			{
				includes.insert( getIncludeForData( inputs[i].key_values.keyFormat ) );
				includes.insert( getIncludeForData( inputs[i].key_values.valueFormat ) );
			}
		}
		
		
		std::string getCompareFunction()
		{
			std::ostringstream output;
			
			output << "\tint " << getCompareFunctionName() << "(KV* kv1 , KV* kv2)" << std::endl;
			output << "\t{" << std::endl;

			output << "\t\tint res_key = " << getCompareFunctionForData( inputs[0].key_values.keyFormat, inputs[0].compareKeyFunction ) << "(kv1->key , kv2->key);\n";
			
			output << "\t\tif(res_key!=0)\n";
			output << "\t\t\treturn (res_key<0);\n";

			output << "\t\tif( kv1->input != kv2->input )\n";
			output << "\t\t\treturn (kv1->input < kv2->input);\n";
			
			output << "\t\tswitch (kv1->input) {\n";
			
			for (size_t i = 0 ; i < inputs.size() ; i++)
				output << "\t\tcase "<<i<<": return ("<< getCompareFunctionForData( inputs[i].key_values.valueFormat, inputs[i].compareValueFunction ) <<"(kv1->value , kv2->value)<0); break;\n";
			
			output << "\t\tdefault: exit(1); break;\n";
			output << "\t\t}\n";
			
			output << "\t\treturn 0;\n";
			
			output << "\t}" << std::endl;
			
			return output.str();
		}
		
		std::string getCompareFunctionName()
		{
			std::ostringstream output;
			output << "compare_" << module << "_" << name;
			return output.str();
		}

		std::string getCompareByKeyFunction()
		{
			std::ostringstream output;
			
			output << "\tint " << getCompareByKeyFunctionName() << "(KV* kv1 , KV* kv2)" << std::endl;
			output << "\t{" << std::endl;
			output << "\t\treturn " << getCompareFunctionForData( inputs[0].key_values.keyFormat, inputs[0].compareKeyFunction) << "(kv1->key , kv2->key);\n";
			output << "\t}" << std::endl;
			
			return output.str();
		}
		
		std::string getCompareByKeyFunctionName()
		{
			std::ostringstream output;
			output << "compare_" << module << "_" << name << "_by_key";
			return output.str();
		}		
		
		
		
	};
}

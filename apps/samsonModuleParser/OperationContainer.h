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

#include <samson/KVFormat.h>
#include "DataType.h"



namespace ss
{
	class DataCreator;
	class AUTockenizer;
	
	/** 
	 Container to hold information for an operation
	 */
	
	class OperationContainer
	{
		
	public:
		
		bool setup;
		std::string name;
		
		std::string my_namespace;
		
		std::string type;
		std::vector <KVFormat> inputs;
		std::vector <KVFormat> outputs;
		
		std::string helpLine;				// Help in one line
		std::vector <std::string> help;		// More extended help
		
		std::string code;		// More extended help
		std::string file;		// File where this operation is defined
		std::vector <std::string> functions; 
		bool top;
		bool dynamic_input_formats;
		bool destructor;
		
		OperationContainer( std::string _type, std::string _name , std::string _my_namespace )
		{
			type = _type;
			name = _name; 
			my_namespace = _my_namespace;
			
			setup = false;
			top =  false;
			dynamic_input_formats = false;
			destructor = false;
		}
		
		void parse( AUTockenizer *module_creator , int begin , int end );
		
		std::string parentClass()
		{
			if( type == "generator")
				return "ss::Generator";
			
			if( type == "map")
				return "ss::Map";
			
			if( type == "reduce")
				return "ss::Reduce";

			if( type == "script")
				return "ss::Script";
			
			fprintf(stderr, "Error: Unknown type of operation in the operation section (%s)\n" , type.c_str());
			_exit(0);
			
		}
		
		std::string className()
		{
			return name;
		}
		
		std::string mainFunctionToImplement(bool fullNameSpace)
		{
			std::ostringstream o;
			
			std::string nameSpaceLocal = "";
			if( fullNameSpace )
				nameSpaceLocal = my_namespace + name + "::";
			
			if( (type == "map") || (type == "reduce") )
				o << "void " << nameSpaceLocal <<    "run(  ss::KVSetStruct* inputs , std::vector<ss::KVWriter*>& outputs )";
			if( type == "generator" )
				o << "void "<< nameSpaceLocal << "run( std::vector<ss::KVWriter *>& writers )";
			
			return o.str();
			
		}
		
		
		void printClassDefinition(FILE *file )
		{
			std::cout << "Creating class " << className().c_str() << std::endl;
			
			// Pragma label
			fprintf(file, "#pragma mark CLASS %s : %s %s\n\n", className().c_str() , type.c_str() , name.c_str() );
			
			// Class definition
			fprintf(file, "\nclass %s : public %s {\n", className().c_str() , parentClass().c_str() );
			
			// Public tag
			fprintf(file, "\n\tpublic:\n");

			if( type != "script")
			{
				// Extern function definition
				for (vector <std::string>::iterator function = functions.begin() ; function != functions.end() ; function++)
					fprintf(file, "\t%s\n", (*function).c_str() );
				fprintf(file, "\n");
			
				//Main run function
				fprintf(file, "\n\t//Main function to implement\n");
				fprintf(file, "\t%s;\n",mainFunctionToImplement( false ).c_str() );

			}

				
				
			// Help of this function
			fprintf(file, "\n\tstd::string help(){\n");
			fprintf(file, "\t\tstd::ostringstream o;\n");
			for( std::vector<std::string>::iterator iter = help.begin() ; iter < help.end() ; iter++)
				fprintf(file, "\t\to<<\"%s\\n\";\n" , (*iter).c_str());
			fprintf(file,"\t\treturn o.str();\n");		
			fprintf(file, "\t}\n");
			

			printStaticOperationFunction(file);
			
			// Help to show what should be implemented
			if( type != "script")
			{
				fprintf(file, "\n\n\t// Implement this functions:\n");
				fprintf(file, "\t//%s{};\n",mainFunctionToImplement( true ).c_str() );
			}
			
			// End of the class
			fprintf(file, "};\n");
			
		}
		
		void printStaticOperationFunction(FILE *file)
		{
			// Static function to create the object to add to the module
			
			std::ostringstream output;
			if( type == "script")
				output << "\t\tss::Operation * operation = new ss::Operation( \"" << name << "\" , ss::Operation::"<< type <<");"<<std::endl;
			else
				output << "\t\tss::Operation * operation = new ss::Operation( \"" << name << "\" , ss::Operation::"<< type <<" , au::factory<"<< className() <<"> );"<<std::endl;
			
			for (size_t i = 0 ; i < inputs.size() ; i++)
				output << "\t\toperation->inputFormats.push_back( ss::KVFormat::format(\""<< inputs[i].keyFormat <<"\" ,\"" << inputs[i].valueFormat << "\") );"<<std::endl;
			
			for (size_t i = 0 ; i < outputs.size() ; i++)
				output << "\t\toperation->outputFormats.push_back( ss::KVFormat::format(\""<< outputs[i].keyFormat <<"\" ,\"" << outputs[i].valueFormat << "\") );"<<std::endl;
			
			
			// Help
			output << "\t\toperation->setHelpLine(\"" << helpLine << "\");\n";
			
			// Full help
			output << "\t\tstd::ostringstream o;\n";
			for (size_t i = 0 ; i < help.size() ; i++)
				output << "\t\to << \"" << help[i] << "\\n\";\n";
			output << "\t\toperation->setHelp( o.str() );\n";
			
			// Code if any
			if ( code.length() > 0)
			{
				output << std::endl;
				output << "\t\t// Code of this operation (usually scripts)\n";
				output << std::endl;
				std::ostringstream command;
				for (size_t i = 0 ; i < code.length() ;i++)
				{
					if( code[i] != '\t')
					{
						if( code[i] == '\n' )
						{
							if( command.str().length()>0)
								output << "\t\toperation->code.push_back(\"" << command.str() <<  "\");\n";
							command.str("");
						}
						else
							command << code[i];
					}
				}
				if( command.str().length()>0)
					output << "\t\toperation.code.push_back(\"" << command.str() <<  "\");\n";
				output << std::endl;

			}
			
			output << "\t\treturn operation;"<<std::endl;
			
			fprintf(file, "\tstatic ss::Operation* operation(){\n");
			fprintf(file, "%s", output.str().c_str() );
			fprintf(file, "\t}\n");
			
		}
		
		void printMainFunctionDefinition( FILE * file)
		{
			if( type == "script" )
				return;
			
			// Help to show what should be implemented
			fprintf(file, "//%s\n" , name.c_str() );
			fprintf(file, "//%s{};\n",mainFunctionToImplement( true ).c_str() );
		}
	};
}

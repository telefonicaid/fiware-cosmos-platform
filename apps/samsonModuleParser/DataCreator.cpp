/* ****************************************************************************
*
* FILE                     DataCreator.cpp
*
* DESCRIPTION			   Creation of Data headers
*
*/
#include "DataCreator.h"		// Own interface
#include "AUTockenizer.h"		// AUTockenizer



namespace ss {

	DataCreator::DataCreator( std::string _moduleFileName  )
	{

		size_t pos = _moduleFileName.find_last_of( "/" );

		if( pos == string::npos )
		{
			directory = "";
			moduleFileName = _moduleFileName;
		}
		else
		{
			directory = _moduleFileName.substr( 0 , pos+1 );
			moduleFileName = _moduleFileName.substr( pos+1 , _moduleFileName.size() - (pos+1 ) );
		}
		
		outputFileName = "Module";
		
		std::cout << "Input module file " << moduleFileName << std::endl;
		std::cout << "Output module file " << outputFileName << std::endl;
		std::cout << "Base direcotry " << directory << std::endl;
		
		module = NULL;
		
		readFile();
		parse();
	}

	void DataCreator::readFile( )
	{
		// Reset content
		content.clear();
		
		
		ifstream input;
		std::string fileName = directory + moduleFileName;
		input.open( fileName.c_str() );
		
		if( !input.is_open() )
		{
			std::cerr << "It was not possible to open the file " << fileName << "\n";
			exit(0);
			return;
		}
		
		ostringstream o;
		char buffer[1001];
		while( !input.eof() )
		{
			input.read( buffer , 1000);
			int num = input.gcount();
			buffer[num] = '\0';
			o << buffer;
		}
		input.close();
		
		content = o.str();
		
	}


	void DataCreator::parse()
	{
		
		AUTockenizer *t = new AUTockenizer(content);
		
		// Parse contents of the items creating elemntes datas, formats, operations, etc...
		int pos = 0;
		while ( pos < (int)t->items.size() )
		{
			if( t->isSpecial(pos) )
				LM_X(1,("Error parsing data-type since we found an spetial character instead of a name"));

			std::string command = t->itemAtPos(pos++);
			
			if( t->isSpecial(pos) )
				LM_X(1,("Error parsing data-type since we found an spetial character instead of a name"));
			
			std::string name = t->itemAtPos(pos++);
			
			int position_start,position_finish;
			t->getScopeLimits(&pos, &position_start, &position_finish);
			
			if( command == "Module" )
			{
				if( module )
				{
					fprintf(stderr, "Error: Duplicated module section\n");
					exit(0);
				}
				
				module = new ModuleContainer( name );
				module->parse( t, position_start , position_finish );
				
				std::cout << "Defining module " << name << "\n";
			} 
			else if ( command == "data" )
			{
				if( !module)
				{
					fprintf(stderr, "Error: Module section should be the first one in the definition file\n");
					exit(0);
				}
				
				std::cout << "Processing Data " << name << " in module " << module->name << std::endl;
				
				DataContainer data_container( module->name , name );
				data_container.parse( t, position_start , position_finish );
				datas.push_back( data_container );
				
			} 
			else if( 
					( command == "script" ) ||
					( command == "generator" )	|| 
					( command == "map" )		|| 
					( command == "parser" )		|| 
					( command == "parserOut" )	|| 
					( command == "reduce" )
					)
			{
				
				if( !module)
				{
					fprintf(stderr, "Error: Module section should be the first one in the definition file\n");
					exit(0);
				}
				
				std::cout << "Processing " << command << " operation " << name << std::endl;
				OperationContainer operation_container( module->name , command, name );
				operation_container.parse( t, position_start , position_finish );
				operations.push_back( operation_container );
			}
			else
			{
				fprintf(stderr, "Unknown command %s\n", command.c_str() );
				exit(0);
			}
		}
		
		delete t;
	}

	
	
	void DataCreator::printMainHeaderFile()
	{
		
#pragma mark MODULE .h file
		
		std::string data_file_name = directory + outputFileName + ".h";
		std::cout << "Creating file " << data_file_name << "\n";
		
		std::ofstream output( data_file_name.c_str() );
		
		output << "\n\n";
		
		output << "/*\n\n\tModule "<< module->title << " (" << module->name << ")\n\n";
		output << "\tFile: " << data_file_name << "\n";
		output << "\tNOTE: This file has been generated with the samson_module tool, please do not modify\n\n";
		output << "*/\n\n";
		
		output << "\n\n";
		
#pragma mark IFDEF 
		
		output << "#ifndef " << module->getDefineUniqueName() << "\n";
		output << "#define " << module->getDefineUniqueName() << "\n";
		
		output << "\n\n";
		
#pragma mark INCLUDES			
		
		output << "\n#include <samson/samsonModuleHeader.h>\n";
		
		// Include all data files	
		for (size_t i = 0 ; i < datas.size() ; i++)
			output << "#include " << datas[i].getIncludeFile() << "\n";

		// Include all operation files	
		for (size_t i = 0 ; i < operations.size() ; i++)
			output << "#include \"operations/" << operations[i].name << ".h\"\n";
		
		output << "\n\n";
		
#pragma mark NAMESPACE
		
		output << "namespace ss{\n";
		output << "namespace " << module->name << "{\n";

#pragma mark MODULE
		
		output << "\n// Module definition\n\n";	  
		output << "\tclass "<< module->getClassName() << " : public ss::Module\n";

		output << "\t{\n";
		output << "\tpublic:\n";
		
		output << "\t\t" << module->getClassName() << "();\n";
		
		// Help of this function
		output << "\n\t\tstd::string help(){\n";
		output << "\t\t\tstd::ostringstream o;\n";
		for( std::vector<std::string>::iterator iter = module->help.begin() ; iter < module->help.end() ; iter++)
			output << "\t\t o<<\"" <<(*iter)<< "\";\n";
		output << "\t\t\treturn o.str();\n";

		output << "\t\t}\n";
		
		output << "\t};\n";
		
		//End of all definition
		
		output << "\n\n";
		
#pragma mark END NAMESPACE
		
		output << "} // end of namespace " << module->name << "\n";
		output << "} // end of namespace ss\n\n" ;

		output << "\n\n";
		
		output << "#endif\n";  
		
		output.close();
		
	}

	
	void DataCreator::printMainFile()
	{
		
		// Print .cpp file for module definition
		std::string output_filename_cpp = directory + outputFileName + ".cpp";
		std::cout << "Creating file " << output_filename_cpp << "\n";
		
		std::ofstream output( output_filename_cpp.c_str() );
		
		output << "\n\n";
		
		
		output << "/**\n";
		output << "\tThis file has been autogenerated using samsonModuleParser." << std::endl;
		output << "\tPlease, do not edit!" << std::endl;
		output << "*/\n";
		
		output << "\n\n";

		output << "#include \"" << outputFileName << ".h\"" << std::endl;
		
		
		std::set<std::string> includes ;
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
		{
			OperationContainer op = *iter;
			if( op.type == "reduce")
				op.getIncludes( includes );
		}
		for( std::set<std::string>::iterator iter = includes.begin() ; iter != includes.end() ; iter++)
			output << *iter;
		
		
		
		output << "\n\n";
		
		output << "extern \"C\" {" << std::endl;
		output << "\tss::Module * moduleCreator( )" << std::endl;
		output << "\t{" << std::endl;
		output << "\t\treturn new " << module->getFullClassName() << "();" << std::endl;
		output << "\t}" << std::endl;
		output << "\tstd::string getSamsonVersion()" << std::endl;
		output << "\t{" << std::endl;
		output << "\t\treturn SAMSON_VERSION;" << std::endl;
		output << "\t}" << std::endl;
		output << "}\n\n" << std::endl;
		
		
		output << "namespace ss{\n";
		output << "namespace "<< module->name <<"{\n";

		
		output << "\n\n";
		output << "// Comparisson function for reduce operations\n";
		
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
		{
			OperationContainer op = *iter;
			if( op.type == "reduce")
			{
				output << op.getCompareFunction();
				output << op.getCompareByKeyFunction();
			}
		}
			
		output << "\n\n";
		
		output << "\t" << module->getClassName() << "::" << module->getClassName() << "()";
		output << " : ss::Module(\"" << module->name << "\",\"" << module->version << "\",\"" << module->author << "\")\n";
		output << "\t{";
		
		output << "\n";
		output << "\t\t//Add datas\n";
		for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
		{
			output << "\t\tadd( new Data(\""<< iter->module << "." << iter->name <<"\" ,";
			output << "getDataIntace<"<< iter->mainClassName()<<">, ";
			output << iter->mainClassName() << "::size )";
			output << ");\n";
		}

		output << "\n";
		output << "\t\t//Add operations\n";
		
		// Add Operations
		
#pragma mark Static operation
		
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
		{
			OperationContainer op = *iter;
			

			output << "\t\t{\n";
			
			if( op.type == "script")
				output << "\t\t\tss::Operation * operation = new ss::Operation( \"" << op.module << "." << op.name << "\" , ss::Operation::"<< op.type <<");"<<std::endl;
			else if( op.type == "reduce")
				output << "\t\t\tss::Operation * operation = new ss::Operation( \"" << op.module << "." << op.name << "\" , ss::Operation::"<< op.type <<" , au::factory<"<< op.name <<">, " << op.getCompareFunctionName() <<  ", " << op.getCompareByKeyFunctionName() <<  " );"<<std::endl;
			else
				output << "\t\t\tss::Operation * operation = new ss::Operation( \"" << op.module << "." << op.name << "\" , ss::Operation::"<< op.type <<" , au::factory<"<< op.name <<"> );"<<std::endl;
			
			
			if( op.type == "parser")
				output << "\t\t\toperation->inputFormats.push_back( ss::KVFormat::format(\"txt\" ,\"txt\") );"<<std::endl;
			
			
			for (size_t i = 0 ; i < op.inputs.size() ; i++)
				output << "\t\t\toperation->inputFormats.push_back( ss::KVFormat::format(\""<< op.inputs[i].keyFormat <<"\" ,\"" << op.inputs[i].valueFormat << "\") );"<<std::endl;
			
			for (size_t i = 0 ; i < op.outputs.size() ; i++)
				output << "\t\t\toperation->outputFormats.push_back( ss::KVFormat::format(\""<< op.outputs[i].keyFormat <<"\" ,\"" << op.outputs[i].valueFormat << "\") );"<<std::endl;
			
			
			// Help
			output << "\t\t\toperation->setHelpLine(\"" << op.helpLine << "\");\n";
			
			// Full help
			output << "\t\t\tstd::ostringstream o;\n";
			for (size_t i = 0 ; i < op.help.size() ; i++)
				output << "\t\t\to << \"" << op.help[i] << "\\n\";\n";
			output << "\t\t\toperation->setHelp( o.str() );\n";
			
			// Code if any
			if ( op.code.length() > 0)
			{
				output << std::endl;
				output << "\t\t\t// Code of this operation (usually scripts)\n";
				output << std::endl;
				std::ostringstream command;
				for (size_t i = 0 ; i < op.code.length() ;i++)
				{
					if( op.code[i] != '\t')
					{
						if( op.code[i] == '\n' )
						{
							if( command.str().length()>0)
								output << "\t\t\toperation->code.push_back(\"" << command.str() <<  "\");\n";
							command.str("");
						}
						else
							command << op.code[i];
					}
				}
				if( command.str().length()>0)
					output << "\t\t\toperation->code.push_back(\"" << command.str() <<  "\");\n";
				output << std::endl;
				
			}

			output << "\t\t\tadd( operation ); // Add this operation to the module\n";
			output << "\t\t}\n";
		}
		
		
#pragma mark  END OF ADDING OPERATIONS
		
		
		output << "\n";
		 
		output << "\t}; // end of class\n";
		
		output << "\n";

		output << "} // end of namespace ss\n";
		output << "} // end of namespace " << module->name <<"\n";;
		
		
		// Implementation of the Module constructor
		output.close();
		
	}
	
	void DataCreator::print()
	{
		// Generate the data files 
		for (size_t i = 0 ; i < datas.size() ; i++)
			datas[i].printFiles(directory);

		// Generate the operation files 
		for (size_t i = 0 ; i < operations.size() ; i++)
			operations[i].printFile(directory);
		
		
		// Print the header file
		printMainHeaderFile();
		
		// Print the main .cpp file
		printMainFile();

	}
}

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

	DataCreator::DataCreator( std::string _moduleFileName , std::string _outputFileName )
	{
		moduleFileName = _moduleFileName;
		outputFileName = _outputFileName;
		
		module = NULL;
		
		readFile();
		parse();
	}

	void DataCreator::readFile( )
	{
		// Reset content
		content.clear();
		
		
		ifstream input;
		input.open( moduleFileName.c_str() );
		
		if( !input.is_open() )
		{
			fprintf(stderr, "Is was not possible to open the file\n");
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
			assert( !t->isSpecial(pos) );
			std::string command = t->itemAtPos(pos++);
			assert( !t->isSpecial(pos) );
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
			} 
			else if ( command == "data" )
			{
				if( !module)
				{
					fprintf(stderr, "Error: Module section should be the first one in the definition file\n");
					exit(0);
				}
				
				std::cout << "Processing Data " << name << std::endl;
				
				DataContainer data_container( name );
				data_container.parse( t, position_start , position_finish );
				datas.push_back( data_container );
				
			} 
			else if( 
					( command == "script" ) ||
					( command == "generator" )	|| 
					( command == "map" )		|| 
					( command == "reduce" )
					)
			{
				
				if( !module)
				{
					fprintf(stderr, "Error: Module section should be the first one in the definition file\n");
					exit(0);
				}
				
				std::cout << "Processing " << command << " operation " << name << std::endl;
				OperationContainer operation_container( command, name , "ss::" + module->nickname + "::" );
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


	void DataCreator::print()
	{
		
		std::string data_file_name = outputFileName + ".h";
		
		FILE *file = fopen( data_file_name.c_str() , "w" );
		
		fprintf(file, "/*\n\n\tModule %s (%s)\n\n",module->title.c_str() , module->nickname.c_str() );
		fprintf(file, "\tFile: %s.h\n",  outputFileName.c_str());
		fprintf(file, "\tNOTE: This file has been generated with the samson_module tool, please do not modify\n\n");
		fprintf(file, "*/\n\n");
		
		
	#pragma mark IFDEF 
		
		fprintf(file, "#ifndef %s\n", module->getDefineUniqueName().c_str() );
		fprintf(file, "#define %s\n\n", module->getDefineUniqueName().c_str() );
		
		
	#pragma mark INCLUDES			
		
		fprintf(file, "\n#include <samson/samsonModuleHeader.h>\n\n");
		
		// Auxiliar includes (defined in the module section)
		
		for ( std::vector<std::string>::iterator iter = module->includes.begin() ; iter < module->includes.end() ; iter++)
		{
			std::string include_name = *iter;
			if ( include_name[0] == '<' )
				fprintf(file, "#include %s\n", include_name.c_str() );
			else
				fprintf(file, "#include \"%s\"\n", include_name.c_str() );
		}

		
		
		fprintf(file,"\n\n");
		
	#pragma mark NAMESPACE
		
		fprintf(file, "namespace ss\n{\n");
		fprintf(file, "%s",module->getBeginNameSpace().c_str() );
		

		
	#pragma mark DATAS
		
		fprintf(file, "\n// Datas defined in this module\n\n");  
		for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
			iter->printClassDefinition( file );
		
	#pragma mark OPERATION
		
		fprintf(file, "\n// Operations defined in this module\n\n");
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
			iter->printClassDefinition( file   );
		
		
		fprintf(file, "\n// Module definition\n\n");	  
		fprintf(file, "class %s : public ss::Module{\n\n", module->getClassName().c_str());
		fprintf(file, "public:\n");
		
		fprintf(file, "\t\%s();\n",  module->getClassName().c_str() );
		
		// Help of this function
		fprintf(file, "\n\tstd::string help(){\n");
		fprintf(file, "\t\tstd::ostringstream o;\n");
		for( std::vector<std::string>::iterator iter = module->help.begin() ; iter < module->help.end() ; iter++)
			fprintf(file, "\t\to<<\"%s\\n\";\n" , (*iter).c_str());
		fprintf(file, "\t\treturn o.str();\n");
		fprintf(file, "\t}\n");
		
		
		fprintf(file, "};\n");
				
		//End of all definition
		
		fprintf(file, "\n");
		
	#pragma mark END NAMESPACE
		
		fprintf(file, "%s",module->getEndNameSpace().c_str() );
		fprintf(file, "\n\n} // end of namespace ss\n\n" );
		
		fprintf(file, "#endif\n");  
		
		
		fprintf(file, "\n\n// List of functions to be implemented:\n\n");
		
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
		{
			iter->printMainFunctionDefinition( file );
			fprintf(file, "\n");
		}
		
		fclose( file );
		
		
	#pragma mark C++ FILE

		// Print .cpp file for module definition
		
		std::string output_filename_cpp = outputFileName + ".cpp";
		std::ofstream output( output_filename_cpp.c_str() );
		output << "#include \"" << outputFileName << ".h\"" << std::endl;
		output << "//\tThis file has been autogenerated using samsonModuleParser." << std::endl;
		
		
		// Include all the files where data is defined
		std::set<std::string> include_datas;
		for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
			if( iter->file != "" )
				include_datas.insert( iter->file );
		
		for (std::set<std::string>::iterator i = include_datas.begin() ; i != include_datas.end() ; i++)
			output << "#include \""<< *i << "\"\n";
		
		
		output << "//\tDo not edit!" << std::endl;
		output << std::endl;
		output << std::endl;
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
		output << "namespace "<< module->nickname <<"{\n";
		
		output << module->getClassName() << "::" << module->getClassName() << "()";
		output << " : ss::Module(\"" << module->nickname << "\",\"" << module->version << "\",\"" << module->author << "\") {";
		
		output << "//Add datas\n";
		for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
		{
			output << "\t\tadd( new Data(\""<<iter->name <<"\" ,";
			output << "getDataIntace<"<< iter->mainClassName()<<">, ";
			output << iter->mainClassName() << "::compare," ;
			output << iter->mainClassName() << "::size )";
			output << ");\n";
		}
		
		
		// Insert operations into the container
		output << "\n\t\t//Add operatons\n";
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
			output << "\t\tadd( "<<iter->name<<"::operation() );\n";
		
		
		output << "\t}\n";
		
		output << "\n}\n";
		output << "\n}\n";
		
		
		// Implementation of the Module constructor
		output.close();
		
		// Creack definitions in all implementation files
		
		std::set<std::string> fileNames;
		for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
			fileNames.insert( iter->file);
		
		
		std::cout << "Checking implementation files" << std::endl;
		for ( std::set<std::string>::iterator i = fileNames.begin() ; i!= fileNames.end() ;i++)
		{
			if( (*i).length() == 0)
				continue;
			
			ifstream inputFileStream( (*i).c_str() );
			std::ostringstream outputStream;
			ostringstream inputStream;
			
			if( inputFileStream.is_open() )
				while( !inputFileStream.eof() )
				{
					char line[1001];
					inputFileStream.read(line, 1000);
					line[ inputFileStream.gcount() ] = '\0';
					inputStream << line;
				}
			
			std::string content =  inputStream.str();				
			
			std::cout << "checking implementation file " << *i << " (" << content.length() << " bytes )" << std::endl;
			
			outputStream << content;
			
			// Check all operation that belong to this implementation file
			for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
				if( iter->file == (*i) )
				{
					std::cout << "\tCheck implentation of " << iter->name << "\n";
					
					std::string mainFunction = iter->mainFunctionToImplement(true);
					
					if( content.find( mainFunction ) == std::string::npos )
					{
						std::cout << "\tAdding function: " << mainFunction << "\n";
						
						outputStream << "/**" << std::endl;
						outputStream << std::endl;
						outputStream << "Module: " << module->title << std::endl;
						outputStream << "Operation: " << iter->name << std::endl;
						
						outputStream << "Inputs: ";
						for (int in = 0 ; in < (int)iter->inputs.size() ; in++)
							outputStream << iter->inputs[in].str() << " ";
						outputStream << std::endl;
						
						outputStream << "Outputs: ";
						for (int in = 0 ; in < (int)iter->outputs.size() ; in++)
							outputStream << iter->outputs[in].str() << " ";
						outputStream << std::endl;
						
						
						outputStream << std::endl;
						outputStream << "*/" << std::endl;
						
						outputStream << "\n\n" << mainFunction << "\n{\n\t//Write here yout code\n}\n\n";
					}
					
				}
			
			ofstream outputFileStream( (*i).c_str() );
			outputFileStream << outputStream.str();
		}
	}
}

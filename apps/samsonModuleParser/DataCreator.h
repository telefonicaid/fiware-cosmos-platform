#ifndef DATA_CREATOR_H
#define DATA_CREATOR_H

#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <set>



namespace ss {

	/**
	 General tokenizer element
	 */

	class AUTockenizer
	{
	public:
		// Parsed elements from the text
		std::vector<std::string> items;
		
		AUTockenizer( std::string txt )
		{
			// First tokenize everything
			std::vector<std::string> previous_items = tockenize(txt);
			
			// Remove spaces and returns.
			// Spaces between " " are not removed
			items = removeSpacesAndReturns( removeComments(previous_items) );
		}

		static std::vector<std::string> removeSpacesAndReturns( std::vector<std::string> items )
		{
			std::vector<std::string> new_items;
			
			bool literal = false;
			
			std::vector<std::string>::iterator iter;
			
			for ( iter = items.begin() ; iter < items.end() ; iter++)
			{
				if( literal )
				{
					if ( *iter == "\"" )
						literal = false;
					new_items.push_back( *iter );
					
				}
				else
				{
					if ( *iter == "\"" )
					{
						literal = true;
						new_items.push_back( *iter );
					}
					else
					{
						// Outside literals, we do not have "returns" "spaces" "tabs" "\r"
						if( !isOneOf(*iter, " \t\r\n") )
							new_items.push_back( *iter );
							
					}
					
				}
			}
			
			return new_items;
			
		}

		/** 
		 Function to remove comments from tockens
		 Bascially removes every tocken between # and end_of_line
		 */
		
		
		static std::vector<std::string> removeComments( std::vector<std::string> items )
		{
			std::vector<std::string> new_items;
			
			bool removing = false;
			
			std::vector<std::string>::iterator iter;
			
			for ( iter = items.begin() ; iter < items.end() ; iter++)
			{
				if( removing )
				{
					if ( *iter == "\n" )
					{
						removing = false;
						new_items.push_back( *iter );	//Keep the return
					}
				}
				else
				{
					if ( *iter == "#" )
						removing = true;
					else
						new_items.push_back( *iter );
				}
			}
			
			return new_items;
			
		}
		
		/**
		 
		 Functions to tockenie the input string
		 
		 */
		
		static std::vector<std::string> tockenize( std::string txt)
		{
			std::string tockens = " #\r\t\r\n{};\"";//All possible delimiters
			
			std::vector<std::string> items;
			
			// Simple parser
			size_t pos = 0;
			for (size_t i = 0 ; i < txt.size() ; i++)
			{
				if ( isOneOf( txt[i] , tockens ) )
				{
					if ( i > pos )
						items.push_back(txt.substr(pos, i - pos ));
					
					//Emit the literal with one letter if that is the case
					ostringstream o;
					o << txt[i];
					items.push_back( o.str() );
					
					pos = i+1;
				}
			}
			
			
			return items;
		}
		
		/**
		 Auxiliar functions
		 */
		
		static bool isOneOf( char c , std::string s )
		{
			for (size_t i = 0 ; i < s.size() ; i++)
				if( s[i] == c )
					return true;
			
			return false;
			
		}

		static bool isOneOf( std::string c_string , std::string s )
		{
			if (c_string.size() > 1)
				return false;
			
			char c = c_string[0];
			
			
			for (size_t i = 0 ; i < s.size() ; i++)
				if( s[i] == c )
					return true;
			
			return false;
			
		}
		
		/**
		 Auxiuliar function to get the "i-th" input item
		 */
		
		std::string itemAtPos( unsigned int pos )
		{
			assert( pos >= 0 );
			assert( pos < items.size() );
			
			return items[pos];
		}
		
		
		bool isSpecial( int pos )
		{
			std::string tmp = itemAtPos(pos);
			return isOneOf(tmp, "{};");
		}
		
		bool isOpenSet( int pos )
		{
			return( itemAtPos(pos) == "{" );
		}
		
		bool isCloseSet( int pos )
		{
			return( itemAtPos(pos) == "}" );
		}
		
		bool isSemiColom( int pos )
		{
			return( itemAtPos(pos) == ";" );
		}
		
		bool isOpenCloseLiteral( int pos )
		{
			return( itemAtPos(pos) == "\"" );
		}
		
		int searchSetFinishStartingAt( int pos )
		{
			int number_intern_sets = 1;
			
			assert( isOpenSet(pos) );
			pos++;
			while( number_intern_sets > 0 )
			{
				if( isCloseSet(pos) )
					number_intern_sets--;
				else if( isOpenSet(pos) )
					number_intern_sets++;
				
				pos++;
			}
			
			return (pos-1);
		}
		
		int searchCloseLiteral( int pos )
		{
			pos++;
			while( !isOpenCloseLiteral(pos) )
				pos++;
			
			return pos;
		}
		
		std::string getLiteralInternal( int pos , int pos2)
		{
			ostringstream o;
			for (int i = (pos+1) ; i < pos2 ; i++)
			{
				o << itemAtPos(i);
			}
			return o.str();
		}
		
		std::string getLiteral( int* pos )
		{
			if( isOpenCloseLiteral(*pos) )
			{
				int pos_start = *pos;
				int pos_finish = searchCloseLiteral(*pos);
				*pos = pos_finish + 1;
				return getLiteralInternal(pos_start, pos_finish);
			}
			else
			{
				int pos_item = *pos;
				*pos = *pos + 1;
				return itemAtPos(pos_item);
			}
		}
		
		
		void getScopeLimits( int* pos , int*begin, int*end )
		{		
			assert( isOpenSet(*pos) );
			*begin = *pos + 1;

			int tmp	= searchSetFinishStartingAt( *pos ) ;
			
			*end = tmp - 1;
			*pos = tmp + 1;
			
		}
	};

	
	/** 
	 Main class of samson_module
	 */

	class DataCreator
	{
	 public:

		//Name of the module
		std::string fileName;
		
		// Read the content of the file into a string
		std::string content;

		// Module object created from the parse
		ModuleContainer *module;
	  
		// List of parsed datas
		vector <DataContainer> datas;
		vector <OperationContainer> operations;
	  

		/* 
		 Constructor: name of the file to parse
		 */
		
		DataCreator( std::string _fileName )
		{
			fileName = _fileName;
			
			module = NULL;

			// Steps of the process
			readFile();
			parse();
		  
		}
	  
		
		/**
		 Read the content of the file into a string
		 */
		
		void readFile( )
		{
			// Reset content
			content.clear();
			
			
			ifstream input;
			input.open( fileName.c_str() );
			
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
		
		/**
		 Parse content
		 Simple parser where all the commands are Command Name { definition  }
		 */
		
		void parse()
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
		
		

		/**
		 Print two two output files *.h and *.cpp
		 */
	  
		void print()
		{
			
			std::string data_file_name = fileName + ".h";

			FILE *file = fopen( data_file_name.c_str() , "w" );

			fprintf(file, "/*\n\n\tModule %s (%s)\n\n",module->title.c_str() , module->nickname.c_str() );
			fprintf(file, "\tFile: %s.h\n",  fileName.c_str());
			fprintf(file, "\tNOTE: This file has been generated with the samson_module tool, please do not modify\n\n");
			fprintf(file, "*/\n\n");

			
#pragma mark IFDEF 
			
			fprintf(file, "#ifndef %s\n", module->getDefineUniqueName().c_str() );
			fprintf(file, "#define %s\n\n", module->getDefineUniqueName().c_str() );
			
			
#pragma mark INCLUDES			
			
			fprintf(file, "\n#include <samson/samson_lib.h>\n\n");
			
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
			
			
			fprintf(file, "\n// Datas defined in this module\n\n");  
			for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
			  iter->printClassDefinition( file );

			
			fprintf(file, "\n// Operations defined in this module\n\n");
			for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
				iter->printClassDefinition( file   );
					
			// Module class definition
			
			fprintf(file, "\n// Module definition\n\n");	  
			fprintf(file, "class %s : public ss::Module{\n\n", module->getClassName().c_str());
			fprintf(file, "public:\n");

			fprintf(file, "\t\%s() : ss::Module(\"%s\",\"%s\",\"%s\"){\n", 
					module->getClassName().c_str() , 
					module->nickname.c_str(), 
					module->version.c_str(),
					module->author.c_str());



			// Insert datas into the container
			
			fprintf(file, "\n\n\t\t//Add datas\n");
			
			for ( vector <DataContainer>::iterator iter = datas.begin() ; iter < datas.end() ; iter++)
			{
				fprintf(file, "\t{\n");
				fprintf(file, "\t\tData *data = new Data( \"%s\" , %s::createFunction );\n" , iter->name.c_str() , iter->name.c_str() );
				fprintf(file, "\t\tdata->setCompareByValueFunction( %s::compareByValue );\n" , iter->className().c_str());
				fprintf(file, "\t\tadd( data );\n");
				
				fprintf(file, "\t}\n");
					
			}
			
			
			// Insert operations into the container
			
			fprintf(file, "\n\n\t\t//Add operatons\n");
			for ( vector <OperationContainer>::iterator iter = operations.begin() ; iter < operations.end() ; iter++)
			{
				std::ostringstream output;
				output << "\t\t{" << std::endl;
				
				if( iter->type == "script")
					output << "\t\tss::Operation * operation = new ss::Operation( \"" << iter->name << "\" , ss::Operation::"<< iter->type <<");"<<std::endl;
				else
					output << "\t\tss::Operation * operation = new ss::Operation( \"" << iter->name << "\" , ss::Operation::"<< iter->type <<" , au::factory<"<< iter->className() <<"> );"<<std::endl;
				
				for (size_t i = 0 ; i < iter->inputs.size() ; i++)
					output << "\t\toperation->inputFormats.push_back( ss::KVFormat::format(\""<< iter->inputs[i].keyFormat <<"\" ,\"" << iter->inputs[i].valueFormat << "\") );"<<std::endl;

				for (size_t i = 0 ; i < iter->outputs.size() ; i++)
					output << "\t\toperation->outputFormats.push_back( ss::KVFormat::format(\""<< iter->outputs[i].keyFormat <<"\" ,\"" << iter->outputs[i].valueFormat << "\") );"<<std::endl;

				
				// Help
				output << "\t\toperation->setHelpLine(\"" << iter->helpLine << "\");\n";
				
				// Full help
				std::vector <std::string> *help = &iter->help;
				output << "\t\tstd::ostringstream o;\n";
				for (size_t i = 0 ; i < help->size() ; i++)
					output << "\t\to << \"" << (*help)[i] << "\\n\";\n";
				output << "\t\toperation->setHelp( o.str() );\n";
				
				// Code if any
				if ( iter->code.length() > 0)
				{
					output << "\t\t// Code of this operation (usually scripts)\n";
					std::ostringstream command;
					for (size_t i = 0 ; i < iter->code.length() ;i++)
					{
						if( iter->code[i] != '\t')
						{
							if( iter->code[i] == '\n' )
							{
								if( command.str().length()>0)
									output << "\t\toperation->code.push_back(\"" << command.str() <<  "\");\n";
								command.str("");
							}
							else
								command << iter->code[i];
						}
					}
					if( command.str().length()>0)
						output << "\t\toperation.code.push_back(\"" << command.str() <<  "\");\n";
				}
				
				
				output << "\t\tadd( operation );"<<std::endl;
				output << "\t\t}" << std::endl;
				
				fprintf(file, "%s" , output.str().c_str() );
				
				
			}
			
			
			fprintf(file, "\t}\n");
			
			//Help of this function
			
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
			
			std::string output_filename_cpp = fileName + ".cpp";
			std::ofstream output( output_filename_cpp.c_str() );

			
			output << "#include \"" << fileName << ".h\"" << std::endl;

			output << "/**" << std::endl;
			
			output << "\tThis file has been autogenerated with samson_module." << std::endl;
			output << "\tPlease, do not modify directly" << std::endl;

			output << "*/" << std::endl;
			
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

			output << "}" << std::endl;
			
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
	};
}

#endif

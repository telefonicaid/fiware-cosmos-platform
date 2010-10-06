#ifndef SAMSON_DATA_CONTAINER_H
#define SAMSON_DATA_CONTAINER_H

/* ****************************************************************************
* 
* FILE                      DataContainer.h
* 
* AUTHOR                    Andreu Urruela Planas
*
*
* A definition of a type with a vector of elements insider
* 
*/
#include <unistd.h>             /* _exit, ...                              */
#include <string.h>             /* strtok, strcpy, ...                     */
#include <cstdio>
#include <map>
#include <string>
#include <iostream>
#include <string>
#include <vector>

#include "KVFormat.h"
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
		std::string file;
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
			// If script, not class
			if( type == "script")
				return;
			
			std::cout << "Creating class " << className().c_str() << std::endl;

			// Pragma label
			fprintf(file, "#pragma mark CLASS %s : %s %s\n\n", className().c_str() , type.c_str() , name.c_str() );

			// Class definition
			fprintf(file, "\nclass %s : public %s {\n", className().c_str() , parentClass().c_str() );

			// Public tag
			fprintf(file, "\n\tpublic:\n");
			
			// Extern function definition
			for (vector <std::string>::iterator function = functions.begin() ; function != functions.end() ; function++)
				fprintf(file, "\t%s\n", (*function).c_str() );
			fprintf(file, "\n");
			
			//Main run function
			fprintf(file, "\n\t//Main function to implement\n");
			fprintf(file, "\t%s;\n",mainFunctionToImplement( false ).c_str() );
			
			// Help of this function
			fprintf(file, "\n\tstd::string help(){\n");
			fprintf(file, "\t\tstd::ostringstream o;\n");
			for( std::vector<std::string>::iterator iter = help.begin() ; iter < help.end() ; iter++)
				fprintf(file, "\t\to<<\"%s\\n\";\n" , (*iter).c_str());
			fprintf(file,"\t\treturn o.str();\n");		
			fprintf(file, "\t}\n");

			// Help to show what should be implemented
			fprintf(file, "\n\n\t// Implement this functions:\n");
			fprintf(file, "\t//%s{};\n",mainFunctionToImplement( true ).c_str() );
			
			// End of the class
			fprintf(file, "};\n");
		
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
	

	class ModuleContainer
	{
		
	public:
		
		std::string title;
		std::string nickname;
		std::string author;
		std::string version;
		
		std::vector <std::string> includes;
		std::vector <std::string> help;
		
		std::map<std::string,std::string> sets;
		
		ModuleContainer( std::string _nickName )
		{
			// Nick name could be sna or sna.cdrs or sna.cdrs.spain.... reverse domain
			nickname = _nickName;
		}

#pragma mark DEFINE NAME
		
		std::string getDefineUniqueName()
		{
			std::ostringstream o;
			o << "_H_SS_";
			
			std::vector<std::string> tockens = tockenizeWithDots( nickname.c_str() );	
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << tockens[i] << "_";
			
			o << "Module";
			return o.str();
			
		}
		
		
		
#pragma mark Begin and End namepsace definitions
		
		std::string getClassName()
		{
			return "Module";
		}

		std::string getFullClassName()
		{
			
			std::ostringstream o;
			o << "ss::";
			
			std::vector<std::string> tockens = tockenizeWithDots( nickname.c_str() );	
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << tockens[i] << "::";

			o << "Module";
			return o.str();
		}
		
		
#pragma mark Begin and End namepsace definitions
		

		std::vector<std::string> tockenizeWithDots( std::string myString )
		{
			assert( myString.length() < 1000 );
			char tmp[1000];
			strcpy(tmp, myString.c_str() );
			std::vector<std::string> tockens;
			char *p = strtok(tmp, ".");
			while (p) {
				tockens.push_back( std::string(p) ); 
				p = strtok(NULL, " ");
			}			
			return tockens;
		}
		
		std::string getBeginNameSpace()
		{
			std::vector<std::string> tockens = tockenizeWithDots( nickname.c_str() );	
			std::ostringstream o;
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << "namespace " << tockens[i] << "{\n";
			return o.str();
		}
		
		std::string getEndNameSpace()
		{
			std::vector<std::string> tockens = tockenizeWithDots( nickname.c_str() );	
			std::ostringstream o;
			for (size_t i = 0 ; i < tockens.size() ; i++)
				o << "}\n";
			return o.str();
		}
		
		void parse( AUTockenizer *module_creator ,int begin ,int end );
		
	};

	class DataContainer
	{
		std::vector <DataType> items;
		
	 public:
	  
		std::string name;
		std::vector <std::string> functions; 
		
		DataContainer( std::string _name )
		{
			name = _name; 
			
		}
		
		void addItem( DataType item )
		{
			items.push_back( item );
		}
		
		
		bool parse( AUTockenizer *module_creator , int begin , int end );
		
		std::string className(  )
		{
			return name;
		}
		
		void printClassDefinition(FILE *outputFile)
		{
			
			assert( items.size() > 0 );
			
			fprintf(outputFile , "class %s : public ss::DataInstance{\n\n", className( ).c_str()	);
			
			fprintf(outputFile, "public:\n");
			
			//Field definition
			for (vector <DataType>::iterator field =items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getDeclaration("\t").c_str() );
			fprintf(outputFile, "\n");
			
			//Extern function definition
			for (vector <std::string>::iterator function = functions.begin() ; function != functions.end() ; function++)
				fprintf(outputFile, "\t%s\n", (*function).c_str() );
			fprintf(outputFile, "\n");
			
			// Constructor
			fprintf(outputFile, "\t%s() : ss::DataInstance(){\n", className(  ).c_str() );
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getInitialization("\t\t").c_str() );
			fprintf(outputFile, "\t}\n\n");
			
			// Creation function
			
			fprintf(outputFile, "\tstatic DataInstance *createFunction(){\n");
			fprintf(outputFile, "\t\treturn new %s();\n", name.c_str() );
			fprintf(outputFile, "\t}\n");
			
			// Compare by value function
			
			fprintf(outputFile, "\tinline static bool compareByValue( KV* kv1 , KV* kv2)\n");
			fprintf(outputFile, "\t{\n");
			fprintf(outputFile, "\tsize_t offset_1 = 0;\n");
			fprintf(outputFile, "\tsize_t offset_2 = 0;\n");		
			fprintf(outputFile, "\tint c = compare( kv1->value , kv2->value , &offset_1 , &offset_2 );\n"); 
			fprintf(outputFile, "\treturn (c<0);\n");
			fprintf(outputFile, "\t}\n");
				
			
			
			//Destructor
			fprintf(outputFile, "\t~%s() {\n", className(  ).c_str()   );
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getDestruction("\t\t").c_str() );
			fprintf(outputFile, "\t}\n\n");
			
			//Parse
			fprintf(outputFile, "\tint parse(char *data){\n" );
			fprintf(outputFile, "\t\tint offset=0;\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getParseCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn offset;\n");
			fprintf(outputFile, "\t}\n\n");
			
			
			//Serialization
			fprintf(outputFile, "\tint serialize(char *data){\n");
			fprintf(outputFile, "\t\tint offset=0;\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getSerializeCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn offset;\n");
			fprintf(outputFile, "\t}\n\n");
			
			//Size
			fprintf(outputFile, "\tint size(char *data){\n");
			fprintf(outputFile, "\t\tint offset=0;\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getSizeCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn offset;\n");
			fprintf(outputFile, "\t}\n\n");
			
			//Get partition function
			fprintf(outputFile, "\tint getPartition(int max_num_partitions){\n");
			//Only get partition with the fist field
			fprintf(outputFile, "%s", (*items.begin()).getPartitionCommand("\t\t").c_str() );
			fprintf(outputFile, "\t}\n\n");
			
			//Comparison
			fprintf(outputFile, "\tinline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getCompareCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn 0; //If everything is equal\n");
			fprintf(outputFile, "\t}\n\n");
			
			
			//Vector functions
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				if( (*field).vector )
				{
					fprintf(outputFile, "%s", (*field).getSetLengthFunction("\t").c_str() );
					fprintf(outputFile, "%s", (*field).getAddFunction("\t").c_str() );
				}
			}
			
			//Copy from
			fprintf(outputFile, "\tvoid copyFrom( %s *other ){\n", className().c_str());
			
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getCopyFromCommand("\t\t").c_str() );
			
			fprintf(outputFile, "\t};\n");
			
			// str
			fprintf(outputFile, "\n\tstd::string str(){\n");
			fprintf(outputFile, "\t\tstd::ostringstream o;\n");
			
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
			{
				fprintf(outputFile, "%s", (*field).getToStringCommand("\t\t").c_str() );
				fprintf(outputFile, "\t\to<<\" \";\n");
			}
			
			fprintf(outputFile, "\t\treturn o.str();\n");
			fprintf(outputFile, "\t}\n\n");
			
			
			
			fprintf(outputFile , "}; //class %s\n\n", className(  ).c_str()	);	
		}
		
	  
	};

}

#endif



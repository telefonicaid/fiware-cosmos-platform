/* ****************************************************************************
 *
 * FILE                     DataContainer.h
 *
 * DESCRIPTION				Information about a data type (module definition)
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

	class DataContainer
	{
		std::vector <DataType> items;
		
	 public:
	  
		std::string name;
		std::string file;		// File where the main class is defined ( to implement adhoc functions over data )
		
		DataContainer( std::string _name )
		{
			name = _name; 
		}
		
		void addItem( DataType item )
		{
			items.push_back( item );
		}
		
		
		bool parse( AUTockenizer *module_creator , int begin , int end );
		
		std::string mainClassName()
		{
			return name;
		}

		std::string baseClassName()
		{
			return "base_" + name;
		}
		
		void printClassDefinition(FILE *outputFile)
		{
			
			
			assert( items.size() > 0 );
			fprintf(outputFile , "\nclass %s;\n\n", mainClassName( ).c_str()	);

			fprintf(outputFile , "class %s : public ss::DataInstance{\n\n", baseClassName( ).c_str()	);
			
			fprintf(outputFile, "public:\n");
			
			//Field definition
			for (vector <DataType>::iterator field =items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getDeclaration("\t").c_str() );
			fprintf(outputFile, "\n");
						
			// Constructor
			fprintf(outputFile, "\t%s() : ss::DataInstance(){\n", baseClassName(  ).c_str() );
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getInitialization("\t\t").c_str() );
			fprintf(outputFile, "\t}\n\n");
			
			//Destructor
			fprintf(outputFile, "\t~%s() {\n", baseClassName(  ).c_str()   );
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
			fprintf(outputFile, "\tstatic inline int size(char *data){\n");
			fprintf(outputFile, "\t\tint offset=0;\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getSizeCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn offset;\n");
			fprintf(outputFile, "\t}\n\n");
			
			//Get partition function
			fprintf(outputFile, "\tint hash(int max_num_partitions){\n");
			//Only get partition with the fist field
			fprintf(outputFile, "%s", (*items.begin()).getPartitionCommand("\t\t").c_str() );
			fprintf(outputFile, "\t}\n\n");
			
			//Comparison
			fprintf(outputFile, "\tinline static int compare(char * data1 , char *data2 , size_t *offset1 , size_t *offset2 ){\n");
			for (vector <DataType>::iterator field = items.begin() ; field != items.end() ; field++)
				fprintf(outputFile, "%s", (*field).getCompareCommand("\t\t").c_str() );
			fprintf(outputFile, "\t\treturn 0; //If everything is equal\n");
			fprintf(outputFile, "\t}\n\n");
			
			// Global compare function
			fprintf(outputFile,"\tinline static int compare( KV* kv1 , KV*kv2 )\n");
			fprintf(outputFile,"\t{\n");
			fprintf(outputFile,"\t\tsize_t offset_1=0;\n");
			fprintf(outputFile,"\t\tsize_t offset_2=0;\n");
			fprintf(outputFile,"\t\treturn compare( kv1->key , kv2->key , &offset_1 , &offset_2 );\n");
			fprintf(outputFile,"\t}\n\n");
			
			
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
			fprintf(outputFile, "\tvoid copyFrom( %s *other ){\n", baseClassName().c_str());
			
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
			
			
			fprintf(outputFile , "}; //class %s\n\n", baseClassName().c_str()	);	
			
			
			
			if( file == "")
			{
				// If no extended class, define here the main class
				fprintf(outputFile , "class %s : public %s{};\n", mainClassName().c_str() , baseClassName( ).c_str()	);
			}else
			{
				fprintf(outputFile, "// Implementation of %s in %s\n" , mainClassName().c_str() , file.c_str() );
			}
			
		}
		
	  
	};

}




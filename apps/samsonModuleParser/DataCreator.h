/* ****************************************************************************
 *
 * FILE                     DataCreator.h
 *
 * DESCRIPTION				Creation of Data headeres
 *
 * ***************************************************************************/

#pragma mark once

#include <string>
#include <cstdlib>
#include <sys/stat.h>
#include <fstream>
#include <set>
#include <vector>
#include "DataContainer.h"		// DataContainer
#include "ModuleContainer.h"	// ModuleContainer
#include "OperationContainer.h"	// OperationContainer


namespace ss {

	class ModuleContainer;
	class OperationContainer;
	class DataContainer;
	
	class DataCreator
	{
	 public:

		// Directory of the moduleFile to create the rest of elements
		std::string directory;	
		
		// Name of the module
		std::string moduleFileName;

		// Name of the output (.h / .cpp)
		std::string outputFileName;
		
		// Read the content of the file into a string
		std::string content;

		// Module object created from the parse
		ModuleContainer *module;
	  
		// List of parsed datas & operations
		vector <DataContainer> datas;
		vector <OperationContainer> operations;

		/* 
		 Constructor: name of the file to parse
		 */
		
		DataCreator( std::string moduleFileName );
		
		/**
		 Read the content of the file into a string
		 */
		
		void readFile( );
		
		/**
		 Parse content
		 Simple parser where all the commands are Command Name { definition  }
		 */
		
		void parse();
		
		/**
		 Print two two output files *.h and *.cpp
		 */
	  
		void print();
		
		// Print the header file
		void printMainHeaderFile();
		void printMainFile();
		
	};
}


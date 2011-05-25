#ifndef DATATYPE_H
#define DATATYPE_H

/* ****************************************************************************
*
* FILE                     DataType.h
*
* DESCRIPTION				Definition of a basic data-type
*/
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>                   // exit



using namespace std;

namespace samson {

	// Basic operations with "." separated names
	std::vector<std::string> tockenizeWithDots( std::string myString );
	std::string getModuleFromFullName( std::string fullName );
	std::string getNameFromFullName( std::string fullName );
	
	class DataType
	{
	public:
		
		string fullType;
		string module;
		string type;
		string name;
		bool vector;
		
		DataType( std::string _full_type , std::string _name , bool _vector  );
		
		/* Get the inlude file that is necessary to use this data type */
		std::string getInclude();
		
		/* Function to give us the name of a particular class */
		string classNameForType(  );

		/* Function to show the declaration of the field */
		string getDeclaration(string pre_line);
		
		/* Set length function... only in vectors */
		string	getSetLengthFunction( string pre_line );
		
		/* Add element function... only in vectors */
		string	getAddFunction( string pre_line );
		
		/* Initialization inside the constructor */
		string getInitialization(string pre_line);
		
		string getDestruction(string pre_line);
		
		string getParseCommandIndividual( string _name );
		
		string getParseCommand(string pre_line);
		
		string getSerializationCommandIndividual( string _name);
		
		string getSerializeCommand( string pre_line );

		string getSizeCommandIndividual( string _name);
		
		string getSizeCommand(string pre_line);

		string getPartitionCommandIndividual(string _name);
		
		string getPartitionCommand( string pre_line );
		
		string getCompareCommandIndividual( string pre_line ,  string _name );
		
		string getCompareCommand( string pre_line );

		string getToStringCommandIndividual(string _name);
		
		string getToStringCommand( string pre_line );

		string getCopyFromCommandIndividual(string _name);
		
		string getCopyFromCommand(string pre_line);

	};
}

#endif

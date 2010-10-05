#ifndef SAMSON_COMMAND_LINE_H
#define SAMSON_COMMAND_LINE_H

/* ****************************************************************************
*
* FILE                 AUCommandLine.h
*
*  Created by ANDREU URRUELA PLANAS on 4/22/10.
*  Copyright 2010 TID. All rights reserved.
*
* AUCommandLine
* 
* Class used to parse a string containing a command with multiple flags and arguments
* The advantadge of this class is that is supports running-time definiton of possible falgs.
*
* 
* Example: command arg1 -f input.ttx -g 2 arg2
* 
* Depending on how you define flags "f" and "g" this will behavie differently.
*
* You can define flags as "bool", "int" "string" "uint64".
* The properties of each element are:
*   bool:       It is a true/false flag. It does not take any parameter
*   int:        Takes an interguer parameter with it.   Example: -n 12
*   double:     Takes a double parameter.               Example: -threshold 5.67
*   uint64:     Takes an unsigned parameter.            Example: -size 12G , -size 100
*   string:     Takes an atring parameter.              Example: -file andreu.txt
*
* The basic usage has three steps:
*   * Use command set_flag_X( ) methods to define flags and types
*   * Use one of the parse(.) methods to parse the incomming command line
*   * Use get_flag_X methods to access content
*/
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <map>

#include "BufferSizes.h"


// #define AUCommandLine_DEBUG


typedef struct AUCommandLineFlag
{
	std::string type;
	std::string default_value;
	std::string value;
} AUCommandLineFlag;



class AUCommandLine 
{

public:
	
	std::string command;											//!<< Incoming General command
	std::map< std::string , AUCommandLineFlag > flags;				//!<< Map of flags
	std::vector<std::string> arguments;								//!<< Command line arguments separated in "words"
	
	
public:
	
	/** 
	 Simple constructor
	 */
	
	AUCommandLine()
	{
	}
	
	/**
	 Constructor with the provided string
	 */
	AUCommandLine( std::string _command )
	{
		parse(_command );
	}
	
	AUCommandLine(int argc, const char *argv[])
	{
		parse(argc , argv);
	}
	
	
	/**
	 Parse a command line in the classical C-style way
	 */
	
	void parse(int args , const char *argv[] )
	{
		std::ostringstream o;
		for (int i = 0 ; i < args ; i++)
			o << argv[i] << " ";
		
		parse( o.str() );
	}
	
	/**
	 
	 Defining flags
	 
	 */
	
	void reset_flags()
	{
		flags.clear();
		arguments.clear();
	}
	
	void set_flag_boolean( std::string name )
	{
		AUCommandLineFlag flag;
		flag.type = "bool";
		flag.default_value = "false";
		
		flags.insert( std::pair<std::string , AUCommandLineFlag>( name , flag) );
	}
	
	void set_flag_int( std::string name , int default_value )
	{
		AUCommandLineFlag flag;
		flag.type = "int";
		
		std::ostringstream o;
		o << default_value;
		flag.default_value = o.str();
		
		flags.insert( std::pair<std::string , AUCommandLineFlag>( name , flag) );
	}

	void set_flag_string( std::string name , std::string default_value )
	{
		AUCommandLineFlag flag;
		flag.type = "string";
		flag.default_value = default_value;
		
		flags.insert( std::pair<std::string , AUCommandLineFlag>( name , flag) );
	}

	void set_flag_uint64( std::string name , std::string default_value )
	{
		AUCommandLineFlag flag;
		flag.type = "uint64";
		flag.default_value = default_value;
		
		flags.insert( std::pair<std::string , AUCommandLineFlag>( name , flag) );
	}

	void set_flag_uint64( std::string name , size_t default_value )
	{
		AUCommandLineFlag flag;
		flag.type = "uint64";
		
		std::ostringstream o;
		o << default_value;
		flag.default_value = o.str();
		
		flags.insert( std::pair<std::string , AUCommandLineFlag>( name , flag) );
	}
	
	
	
	/** 
	 Parse a commnad line. It extracts all "arguments"
	 */

	
	void parse( std::string _command )
	{
		
#ifdef AUCommandLine_DEBUG
		fprintf(stderr, "Parsing command: %s\n", _command.c_str() );
#endif
		
		clear_values();

		std::vector<std::string> tockens;
		
		// Copy the original command
		command = _command;

		std::string delimiters = " \t,\n";	//All possible delimiters
		
		// Skip delimiters at beginning.
		std::string::size_type lastPos = command.find_first_not_of(delimiters, 0);
		
		// Find first "non-delimiter".
		std::string::size_type pos     = command.find_first_of(delimiters, lastPos);
		
		while (std::string::npos != pos || std::string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tockens.push_back(command.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = command.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = command.find_first_of(delimiters, lastPos);
		}

#ifdef AUCommandLine_DEBUG
		fprintf(stderr, "List of %d tockens\n", tockens.size() );
#endif
		
		
		parse_tockens( tockens );
		
	}	
	
	void clear_values()
	{
		// Remove the "value" field in all "flags"
		std::map< std::string , AUCommandLineFlag >::iterator iter;
		for (iter = flags.begin() ; iter != flags.end() ; iter++)
			iter->second.value = "unknown";	//Default value when no assigned
		
		//Remove the arguments vector
		arguments.clear();
	}
	
	
	void parse_tockens( std::vector<std::string> &tockens )
	{
		// Parse tockens to distinghuish between arguments and flags parameters
		
		std::vector<std::string>::iterator iter;
		
		std::map<std::string,AUCommandLineFlag>::iterator flag_iterator;
		
		
		for (iter = tockens.begin() ; iter < tockens.end() ; iter++)
		{
			std::string tocken = *iter;
			

#ifdef AUCommandLine_DEBUG
			fprintf(stderr, "Processing tocken: %s\n", tocken.c_str() );
#endif
			
			if ( tocken[0] == '-' )
			{
				std::string flag_name = tocken.substr( 1 , tocken.size() -1 );

				flag_iterator = flags.find( flag_name );
				if( flag_iterator != flags.end() )
				{
					if( flag_iterator->second.type == "bool")
					{
						flag_iterator->second.value = "true";
					}
					else
					{
						// This is a unitary parameter
						iter++;
						if( iter != tockens.end() )
						{
							flag_iterator->second.value = *iter;
						}
					}
				}
				
			}
			else
			{
				//Normal argument
				arguments.push_back( tocken );
			}
		}
		
	}

	

	
	/**
	 Acces information about arguments
	 */

	int get_num_arguments()
	{
		return arguments.size();
	}
	
	std::string get_argument( int index )
	{
		if( (index<0) || ( index >= (int)arguments.size() ) )
			return "no-argument";
		
		return arguments[index];
	}
	
	/** 
	 Access to flags
	 */
	 
	std::string getFlagValue( std::string flag_name )
	{
		std::map<std::string,AUCommandLineFlag>::iterator flag_iterator;

		flag_iterator = flags.find( flag_name );
		if( flag_iterator != flags.end() )
		{
			if( flag_iterator->second.value != "unknown" )
				return flag_iterator->second.value;
			else
				return flag_iterator->second.default_value;
		}
		else
			return "unknown_flag";
			
	}

	std::string getFlagType( std::string flag_name )
	{
		std::map<std::string,AUCommandLineFlag>::iterator flag_iterator;
		
		flag_iterator = flags.find( flag_name );
		if( flag_iterator != flags.end() )
			return flag_iterator->second.type;
		else
			return "unknown_type";
		
	}	
	
	/**
	 Specialed access to parametes 
	 */

	bool get_flag_bool( std::string flag_name )
	{
		if( getFlagType(flag_name) != "bool" )
			return false;
		
		return getBoolValue( getFlagValue( flag_name ) );
		
	}
	
	
	int get_flag_int( std::string flag_name )
	{
		if( getFlagType(flag_name) != "int" )
			return 0;
		
		return getIntValue( getFlagValue( flag_name ) );
	}	

	double get_flag_double( std::string flag_name )
	{
		if( getFlagType(flag_name) != "double" )
			return 0;
		
		return getDoubleValue( getFlagValue( flag_name ) );
	}	

	std::string get_flag_string( std::string flag_name )
	{		
		if( getFlagType(flag_name) != "string" )
			return 0;

		return ( getFlagValue( flag_name ) );
	}	

	size_t get_flag_uint64( std::string flag_name )
	{
		if( getFlagType(flag_name) != "uint64" )
			return 0;
		
		return getUint64Value( getFlagValue( flag_name ) );
	}	
	
	/**
	 Functions to transform values
	 */
	
	int getIntValue	( std::string value )
	{
		return atoi( value.c_str() );
	}
	
	size_t getUint64Value	( std::string value )
	{
		size_t base = 1;	//Default base
		
		if ( value[value.size()-1] == 'M' )
		{
			base = AU_SIZE_1M;
			value = value.substr(0, value.size()-1);
		} else if ( value[value.size()-1] == 'G' )
		{
			base = AU_SIZE_1G;
			value = value.substr(0, value.size()-1);
		}
		
		return base * atoi( value.c_str() );
		
	}
	
	double getDoubleValue	(  std::string value )
	{
		return atof( value.c_str() );
	}
	
	bool getBoolValue( std::string value )
	{
		return ( value == "true" );
	}
	
	

};


class AUCommandLineTest
{
public:
	int run()
	{
		if( test1() )
		{
			fprintf(stderr, "Test 1............ OK\n");
			return 0;
		}
		else
		{
			fprintf(stderr, "Test 1............ KO !!!!\n");
			return 1;
		}
		
	}
	

	bool test1()
	{
		std::string test_line = "command -n 1 -m 2 -flag_spetial -f file.txt arg1 -memory 1000 ";
		
		AUCommandLine commandLine;
		
		commandLine.set_flag_int("n", 10);
		commandLine.set_flag_int("m", 11);
		commandLine.set_flag_int("l", 12);
		commandLine.set_flag_uint64("memory", 1024*1024);
		commandLine.set_flag_uint64("memory2", 1024*1024);
		
		commandLine.set_flag_string("f", "unknown_file");


		commandLine.set_flag_boolean("flag_spetial");
		commandLine.set_flag_boolean("flag_spetial2");
		
		commandLine.parse( test_line );


		//Check values at the output
		
		if ( commandLine.get_flag_int("n") != 1)
			return false;
		if ( commandLine.get_flag_int("m") != 2)
			return false;
		if ( commandLine.get_flag_int("l") != 12)
			return false;

		if ( commandLine.get_flag_string("f") != "file.txt")
			return false;

		if ( commandLine.get_flag_uint64("memory") != 1000)
			return false;
		if ( commandLine.get_flag_uint64("memory2") != 1024*1024)
			return false;

		if( !commandLine.get_flag_bool("flag_spetial") )
			return false;

		if( commandLine.get_flag_bool("flag_spetial2") )
			return false;
		
		if ( commandLine.get_num_arguments() != 2)
			return false;

		if ( commandLine.get_argument(0) != "command" )
			return false;
		
		return true;
	}

};

#endif

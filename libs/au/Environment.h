
/* ****************************************************************************
 *
 * FILE            Enviroment.h
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *  Collection of enviroment variables. It is a key-value colection usign strings
 *  Convenient functions are provided to use values as integers, doubles, etc...
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_ENGINE_ENVIRONMENT
#define _H_ENGINE_ENVIRONMENT

#include <math.h>
#include <sstream>         // std::ostringstream
#include <map>			   // std::map
#include <stdlib.h>

namespace au {
	
	class Environment
	{
		
	public:
		
		std::map<std::string,std::string> environment;	
		
        void clearEnvironment();
        
		std::string get( std::string name , std::string default_value );
		void set( std::string name , std::string value );
		void unset( std::string name );
		bool isSet( std::string name );

		void copyFrom( Environment *other );
		
		template<typename T>
		void set( std::string name  , T value)
		{
			std::ostringstream v;
			v << value;
			set( name , v.str() ); 
		}	
		
		void setInt( std::string name  , int value);
		void setSizeT( std::string name  , size_t value);
		void setDouble( std::string name  , double value);
		int getInt( std::string name , int defaultValue);
		size_t getSizeT( std::string name , size_t defaultValue);
		double getDouble( std::string name , double defaultValue);
        
        // Description
		std::string toString();
        std::string getEnvironmentDescription();
		
	};	

}
#endif

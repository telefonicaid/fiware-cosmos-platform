#ifndef _H_ENVIRONMENT
#define _H_ENVIRONMENT

#include <math.h>
#include <sstream>                 // std::ostringstream
#include <map>						// std::map

namespace ss {
	
	/** 
	 Class used to hold environment variables
	 */
	
	class Environment
	{
		
	public:
		
		std::map<std::string,std::string> environment;	
		
		std::string get( std::string name , std::string default_value )
		{
			std::map<std::string, std::string>::iterator iter = environment.find(name);
			
			if( iter == environment.end() )
				return default_value;
			else
				return iter->second;
		}
		
		void set( std::string name , std::string value )
		{
			std::map<std::string, std::string>::iterator iter = environment.find(name);
			
			if( iter == environment.end() )
				environment.insert ( std::pair<std::string, std::string>( name , value ) );
			else
				iter->second = value;
			
		}
		
		bool isSet( std::string name )
		{
			std::map<std::string, std::string>::iterator iter = environment.find(name);
			
			if( iter == environment.end() )
				return false;
			else
				return true;
			
		}
		
		void copyFrom( Environment *other )
		{
			std::map<std::string, std::string>::iterator iter;
			for ( iter = other->environment.begin() ; iter != other->environment.end() ; iter++)
				environment.insert ( std::pair<std::string, std::string>( iter->first , iter->second ) );
		}
		
		
		std::string toString()
		{
			std::ostringstream o;
			
			std::map<std::string, std::string>::iterator iter;
			for (iter = environment.begin() ; iter != environment.end() ; iter++)
				o << iter->first << " : " << iter->second << "\n";
			return o.str();
		}
		
		// Varialbes as ints / size_t / double


		template<typename T>
		void set( std::string name  , T value)
		{
			std::ostringstream v;
			v << value;
			set( name , v.str() ); 
		}	
		
		int getInt( std::string name , int defaultValue)
		{
			if( !isSet(name) )
				return defaultValue;
			return atoi( get( name , "0" ).c_str() );
		}

		size_t getSizeT( std::string name , size_t defaultValue)
		{
			if( !isSet(name) )
				return defaultValue;
			return atoll( get( name , "0" ).c_str() );
		}

		double getDouble( std::string name , double defaultValue)
		{
			if( !isSet(name) )
				return defaultValue;
			return atof( get( name , "0" ).c_str() );
		}
		
	};	

}
#endif

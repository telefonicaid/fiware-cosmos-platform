#ifndef _H_MONITOR_PARAMETER
#define _H_MONITOR_PARAMETER

#include <sstream>
#include "au_map.h"

namespace ss {
	
	template <class T,int n>
	class RRT
	{
		T elements[n];	// Array of values
		
	public:
		
		int pos;	// Current position to write
		
		RRT()
		{
			pos=0;
			for (int i = 0 ; i < n ; i++)
				elements[i] = 0;
		}
		
		bool push( T value )
		{
			elements[pos++] = value;
			if ( pos == n)
			{
				pos = 0;
				return true;
			}
			return false;
		}
		
		T average()
		{
			T total = 0;
			for (int i = 0 ; i < n ; i++)
				total+=elements[i];
			return total/(T)n;
		}
		
		std::string vectorString()
		{
			std::stringstream o;
			o << "[";
			for (int i = 0 ; i < n ; )
			{
				o << "[" << elements[(pos+i)%n]<<"]";
				i++;
				if( i != n)
					o << ",";
			}
			o << "]";
			
			return o.str();
		}
		
	};
	
#pragma mark MonitorParameter
	
	class MonitorParameter 
	{
		size_t currentValue;
		
		RRT<size_t,60> elements;	// A sample every second
		RRT<size_t,60> elements2;	// A sample every minute
		RRT<size_t,24> elements3;	// A sample every hour
		
	public:
		
		void push( size_t value );
		size_t current();
		std::string historyString( int i );	  
	};
	
	
#pragma mark MonitorBlock
	
	class MonitorBlock
	{
		
		std::map<std::string , std::string> main_parameters;
		au::map< std::string , MonitorParameter> parameters;
		
	public:
		
		~MonitorBlock();
		
		void addMainParameter( std::string name , std::string value);		
		void push( std::string key , size_t value );		
		std::string getJSONString();
		
	};
}

#endif

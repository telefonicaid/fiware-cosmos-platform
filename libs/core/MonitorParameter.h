#ifndef _H_MONITOR_PARAMETER
#define _H_MONITOR_PARAMETER

#include <sstream>
#include "au_map.h"
#include "assert.h"			// assert(.)

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


  class MonitorParameter 
  {
		size_t currentValue;

		RRT<size_t,60> elements;	// A sample every second
		RRT<size_t,60> elements2;	// A sample every minute
		RRT<size_t,24> elements3;	// A sample every hour
    
  public:
    
    void push( size_t value )// Push a new sample
    {
		currentValue = value;
		
		if( elements.push( value ) )
			if( elements2.push( elements.average() ) )
				elements3.push( elements2.average() ); 
    }

	  size_t current()
	  {
		  return currentValue;
	  }
	  
	  std::string historyString( int i )
	  {
		  switch (i) {
			  case 0:
				  return elements.vectorString();
				  break;
			  case 1:
				  return elements2.vectorString();
				  break;
			  case 2:
				  return elements3.vectorString();
				  break;
			  default:
				  break;
		  }
		  assert( false);
		  return "";
	  }
	  
  };

	
	
	class MonitorBlock
	{
		
		std::map<std::string , std::string> main_parameters;
		
		au::map< std::string , MonitorParameter> parameters;
		
	public:

		void addMainParameter( std::string name , std::string value)
		{
			main_parameters.insert( std::pair<std::string , std::string>( name , value) );
		}
		
		void push( std::string key , size_t value )
		{
			MonitorParameter *p = parameters.findInMap( key );
			if( !p )
			{
				p = new MonitorParameter();
				parameters.insertInMap( key , p );
			}
			p->push(value);
			
		}
		
		std::string getJSONString()
		{
			std::ostringstream o;
			
			// ------------------------------------------------------------------------------------------------
			{
			o << "{";
				
				std::map< std::string , std::string>::iterator iter;
				for ( iter = main_parameters.begin() ; iter != main_parameters.end() ;iter++ )
				{
					std::string name = iter->first;
					std::string value = iter->second;
					o << name<< ":\"" << value << "\"";
					o << ",";
				}
				
				o << "current:";
				o << "{";
				std::map< std::string , MonitorParameter*>::iterator iterator;
				for (iterator = parameters.begin() ; iterator != parameters.end() ; )
				{
					o << iterator->first << ":" << iterator->second->current();
					
					iterator++;
					if( iterator != parameters.end() )
						o << ",";
				}
				o << "},";
			}
			{
				o << "history:";
				o << "[";
				for (int i = 0 ; i < 3 ; i++)
				{
					
					o << "{";
					std::map< std::string , MonitorParameter*>::iterator iterator;
					for (iterator = parameters.begin() ; iterator != parameters.end() ; )
					{
						o << iterator->first << ":" << iterator->second->historyString(i);
						
						iterator++;
						if( iterator != parameters.end())
							o << ",";
					}
					o << "}";
					
					if( i!= 2)
						o << ",";
				}
				o << "]";
			}
			
			o << "}";
			// ------------------------------------------------------------------------------------------------
			
			return o.str();
		}
		
	};
	
	
	
}

#endif

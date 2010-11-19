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
			for (int i = 0 ; i < n ; i++)
			{
				o << "[" << elements[(pos+i)%n]<<"]";
				o << ",";
			}
			o << "]";

		return o.str();
		}

	};


  class MonitorParameter 
  {
		double currentValue;

		RRT<double,60> elements;	// A sample every second
		RRT<double,60> elements2;	// A sample every minute
		RRT<double,24> elements3;	// A sample every hour
    
  public:
    
    void push( double value )// Push a new sample
    {
		currentValue = value;
		
		if( elements.push( value ) )
			if( elements2.push( elements.average() ) )
				elements3.push( elements2.average() ); 
    }

	  double current()
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
		au::map< std::string , MonitorParameter> parameters;
		
	public:

		void push( std::string key , double value )
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
			o << "{";
			{
				o << "current:";
				o << "{";
				std::map< std::string , MonitorParameter*>::iterator iterator;
				for (iterator = parameters.begin() ; iterator != parameters.end() ; iterator++)
					o << iterator->first << ":" << iterator->second->current() << ",";
				o << "}";
			}
			{
				o << "history:";
				o << "[";
				for (int i = 0 ; i < 3 ; i++)
				{
					
					o << "{";
					std::map< std::string , MonitorParameter*>::iterator iterator;
					for (iterator = parameters.begin() ; iterator != parameters.end() ; iterator++)
						o << iterator->first << ":" << iterator->second->historyString(i) << ",";
					o << "}";
					
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

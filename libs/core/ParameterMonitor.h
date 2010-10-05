#ifndef SAMSON_PARAMETER_MONITOR_H
#define SAMSON_PARAMETER_MONITOR_H

#include <sstream>



namespace ss {
	
	class KVSet;
	class KVQueue;
	
	template <class T,int n>
	class RRT
	{
		T elements[n];
	public:
		int pos;			// Current position to write
		
		
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
		
		std::string str()
		{
			std::stringstream o;
			o << "[";
			for (int i = 0 ; i < n ; i++)
			{
				o << "[" << i << ","<< elements[(pos+i)%n]<<"]";
				if( i != (n-1) )
					o << ",";
			}
			o << "]";
			
			return o.str();
		}
		
	};
	
	class ParameterMonitor 
	{
		RRT<double,60> elements;	// A sample every second
		RRT<double,60> elements2;	// A sample every minute
		RRT<double,24> elements3;	// A sample every hour
		
	public:
		
		// Push a new sample
		void push( double value )
		{
			
			if( elements.push( value ) )
				if( elements2.push( elements.average() ) )
					elements3.push( elements2.average() ); 
		}
		
		std::string str( int scale )
		{
			switch (scale) {
				case 0:
					return elements.str();
					break;
				case 1:
					return elements2.str();
					break;
				case 2:
					return elements3.str();
					break;
				default:
					return "unknown scale";
					break;
			}			
		}		
	};

}

#endif

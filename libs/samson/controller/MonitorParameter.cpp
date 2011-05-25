
#include "MonitorParameter.h"		// Own interface

namespace samson
{
	
		
#pragma mark -
	
	void MonitorParameter::push( size_t value )// Push a new sample
	{
		currentValue = value;
		
		if( elements.push( value ) )
			if( elements2.push( elements.average() ) )
				elements3.push( elements2.average() ); 
	}
	
	size_t MonitorParameter::current()
	{
		return currentValue;
	}
	
	std::string MonitorParameter::historyString( int i )
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
		LM_X(1,("Error at MonitorParameter"));
		return "";
	}
	
#pragma mark MonitorBlock
	
	MonitorBlock::~MonitorBlock()
	{
		parameters.clearMap();
	}
	
	void MonitorBlock::addMainParameter( std::string name , std::string value)
	{
		main_parameters.insert( std::pair<std::string , std::string>( name , value) );
	}
	
	void MonitorBlock::push( std::string key , size_t value )
	{
		MonitorParameter *p = parameters.findInMap( key );
		if( !p )
		{
			p = new MonitorParameter();
			parameters.insertInMap( key , p );
		}
		p->push(value);
		
	}
	
	std::string MonitorBlock::getJSONString()
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
	
}
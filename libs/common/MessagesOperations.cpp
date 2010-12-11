
#include "MessagesOperations.h"		// Own header file


namespace ss {


	bool evalHelpFilter( network::Help *help , std::string name)
	{
		if( help->has_name() )
		{
			if( name == help->name() )
				return true;
			else
				return false;
		}
		
		if( help->has_begin() )
		{
			std::string begin = help->begin();
			
			if( name.length() < begin.length() )
				return false;
			
			return (name.substr(0, begin.length()) == begin);
			
		}
		
		
		return true;
	}
	
	bool filterName( const std::string& name , const std::string& begin , const std::string& end)
	{
		if( begin.length() > 0 )
		{			
			if( name.length() < begin.length() )
				return false;
			
			if (name.substr(0, begin.length()) != begin)
				return false;
		}
		
		if( end.length() > 0 )
		{			
			if( name.length() < end.length() )
				return false;
			
			if (name.substr(name.length()-end.length(), end.length()) != end)
				return false;
		}
		
		return true;
	}

}

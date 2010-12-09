
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

}


#include "samson/common/MessagesOperations.h"		// Own header file


namespace samson {

	
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

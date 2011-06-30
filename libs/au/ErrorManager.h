#ifndef _H_AU_ERROR
#define _H_AU_ERROR

#include <set>
#include <sstream>		// std::ostringstream

namespace au 
{

	class ErrorManager
	{
		bool error;
		std::string message;
		
	public:
		
		ErrorManager();
		
		void set( std::string _message );
		
		bool isActivated();
		
		std::string getMessage(); 
		
        void set( ErrorManager *otherError );
        
        std::string str(); 
        
	};
	
}


#endif

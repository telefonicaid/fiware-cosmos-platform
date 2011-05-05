#ifndef _H_AU_STATUS
#define _H_AU_STATUS

#include <set>
#include <sstream>		// std::ostringstream

namespace au 
{

	class Error
	{
		bool error;
		std::string message;
		
	public:
		
		Error();
		
		void set( std::string _message );
		
		bool isActivated();
		
		std::string getMessage(); 
		
        void set( Error *otherError );
        
        std::string str(); 
        
	};
	
}


#endif

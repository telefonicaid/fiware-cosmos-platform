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
		
		Error()
		{
			error = false;
		}
		
		void set( std::string _message )
		{
			error = true;
			message = _message;
		}
		
		bool isActivated()
		{
			return error;
		}
		
		std::string getMessage()
		{
			return message;
		}
		
	};
	
}


#endif

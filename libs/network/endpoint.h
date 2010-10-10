

/* ****************************************************************************
 *
 * FILE                     endpoint.h
 *
 * DESCRIPTION				EndPoint information about server:port
 *
 * ***************************************************************************/

#pragma once

#include <string>	// std::string...

namespace ss {

	class EndPoint
	{
		std::string description;
		
	public:
		
		EndPoint()
		{
			description = "unknown";
		}

		/**
		 Create an endpoint from a string
		 It is suppoused to be in server:port format
		 */
		
		EndPoint( std::string txt )
		{
			description = txt;
		}
		
		EndPoint( int port)
		{
			description = "localhost" + port;
		}
		
		std::string str()
		{
			return description;
			
		}
		
	};

}

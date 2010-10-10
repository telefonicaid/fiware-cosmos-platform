

/* ****************************************************************************
 *
 * FILE                     endpoint.h
 *
 * DESCRIPTION				EndPoint information about server:port
 *
 * ***************************************************************************/

#pragma once

#include <string>	// std::string...
#include <sstream>	// std::ostringstream

namespace ss {

	class EndPoint
	{
		// ?
		
	public:
		
		EndPoint()
		{
		}
		
		EndPoint( int port)
		{
			// Local endpoint with a port
		
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << "Server:port";
			return o.str();
			
		}
		
	};

}

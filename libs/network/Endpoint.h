

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
		std::string _server;
		int _port;

		bool _valid;				// Flag to indicate if this was a valid endpoint
		
	public:
		
		/**
		 Create an endpoint from a string
		 It is suppoused to be in server:port format
		 */
		
		EndPoint( std::string txt )
		{
			size_t pos = txt.find(":");
			if( pos == std::string::npos )
			{
				_valid = false;
				return;
			}
			
			// Divide txt file to be
			_server = txt.substr( 0 , pos-1);
			_port = atoi( txt.substr( pos+1 , txt.length() - (pos+1) ).c_str() ) ;
		}
		
		EndPoint( int port )
		{
			_server = "localhost";
			_port = port;
		}
		
		std::string str()
		{
			std::ostringstream o;
			o << _server << ":" << _port;
			return o.str();
		}
		
		int port()
		{
			return _port;
		}
		
	};

}

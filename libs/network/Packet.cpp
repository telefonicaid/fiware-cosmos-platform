/* *******************************************************************************
*
* FILE                     Packet.cpp
*
* DESCRIPTION				Definition of the pakcet to be exchange in the samson-ecosystem
*
*/
#include <sstream>		// std::ostringstream

#include "Format.h"		// au::Format
#include "Packet.h"		// Own interface



namespace ss
{
	
	std::string Packet::str()
	{
		std::ostringstream o;
		o << "Packet buffer with " << au::Format::string( buffer.getLength() ) << " bytes"; 
		return o.str();
	}
	
	
}

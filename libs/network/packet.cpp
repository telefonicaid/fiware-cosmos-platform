
/* ****************************************************************************
 *
 * FILE                     packet.h
 *
 * DESCRIPTION				Definition of the pakcet to be exchange in the samson-ecosystem
 *
 * ***************************************************************************/


#include "packet.h"		// Own interface
#include <sstream>		// std::ostringstream
#include "Format.h"		// au::Format

namespace ss
{
	
	std::string Packet::str()
	{
		std::ostringstream o;
		o << "Packet buffer with " << au::Format::string( buffer.getLength() ) << " bytes"; 
		return o.str();
	}
	
	
}
#include "Buffer.h"		// Own interface
#include "Format.h"		// au::Format
#include <sstream>		// std::stringstream

namespace ss {


	std::string Buffer::str()
	{
		std::ostringstream output;
		output << "[ Buffer size:" << au::Format::string( _size ) << " max:" << au::Format::string( _max_size ) ;
		return output.str();
	}

}
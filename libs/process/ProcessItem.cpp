

#include "ProcessItem.h"		// ss::ProcessItem

namespace ss
{

	std::string ProcessItem::getStatus()
	{
		int p = progress*100.0;
		std::ostringstream o;
		
		o << status_letter << std::string(":") << status;
		if ( (p> 0) && (p < 100))
			o << "(" << p << "%)";
		return o.str();
		
	}
	
	
}
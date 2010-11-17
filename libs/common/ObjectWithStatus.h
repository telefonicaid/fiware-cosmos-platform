#ifndef _H_OBJECT_WITH_STATUS
#define _H_OBJECT_WITH_STATUS

#include <string>
#include <map>
#include <sstream>

namespace ss {

	// Plot the status string from a map of elements
	template< typename K, typename V>
	std::string getStatusFromArray(std::map<K,V*>& m)
	{
		std::ostringstream output;
		typename std::map<K, V* >::iterator iter;
		for ( iter = m.begin() ; iter != m.end() ; iter++ )
			output << "\t" << iter->first << " " << iter->second->getStatus() << std::endl;
			return output.str();
	}
			
}

#endif
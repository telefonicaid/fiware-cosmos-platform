#ifndef _H_AU_SET
#define _H_AU_SET

#include <map>          // std::map
#include <list>         // std::list
#include <vector>       // std::vector
#include <set>          // std::set
#include "logMsg.h"		// LM_M()

namespace au {

	/**
	 CLass to use list structures of <class*> with additional function for easy manipulation
	 */
	
	template <class V>
	class set: public std::set<V*>
	{
	public:
		
		void clearSet()
		{
			typename std::set<V* >::iterator iter;
			
			for (iter =  std::set<V*>::begin() ; iter != std::set<V*>::end() ; iter++)
			{
				delete *iter;
			}
			std::set<V*>::clear();
		}
        
	};	
		
  
}

#endif

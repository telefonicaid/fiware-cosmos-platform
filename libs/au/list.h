#ifndef _H_AU_LIST
#define _H_AU_LIST

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set

#include "logMsg.h"					 // LM_M()

namespace au {

	/**
	 CLass to use list structures of <class*> with additional function for easy manipulation
	 */
	
	template <class V>
	class list: public std::list<V*>
	{
	public:
		V* extractFront()
		{
			if( std::list<V*>::size() == 0)
				return NULL;
			else
			{
				V* tmp = std::list<V*>::front();
				std::list<V*>::pop_front();
				return tmp;
			}
		}
		
		V* extractBack()
		{
			if( std::list<V*>::size() == 0)
				return NULL;
			else
			{
				V* tmp = std::list<V*>::back();
				std::list<V*>::pop_back();
				return tmp;
			}
		}
		
		void clearList()
		{
			typename std::list<V* >::iterator iter;
			
			for (iter =  std::list<V*>::begin() ; iter != std::list<V*>::end() ; iter++)
			{
				delete *iter;
			}
			std::list<V*>::clear();
		}
		
		
	};	
  
}

#endif

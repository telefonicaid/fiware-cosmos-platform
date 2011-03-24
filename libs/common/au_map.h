#ifndef _H_AU_MAP
#define _H_AU_MAP

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
	
	/**
	 Class to use map structures of <typedef,class*> with addittional function for easy manitpulation
	 */
	
	template <typename K,class V>
	class map : public std::map<K,V*>
	{
		
	public:

		// Iterator definition
		typename std::map<K, V* >::iterator iter;

		
		// Insert a pair of elements ( easy method )
		// If a previous element was inserted with the same key, it is automatically deleted
		void insertInMap( K& key , V* value)
		{
			V* tmp =  extractFromMap( key );
			if( tmp )
				delete tmp;
			
			insert( std::pair<K,V*>( key, value) );
		}
		
		/*
		 Function to easyly get pointers in a std::map < value , Pointer* >
		 NULL if not found
		 */
		
		V* findInMap( K& key ) 
		{
			typename std::map<K, V* >::iterator iter = find(key);
			
			if( iter == std::map<K,V*>::end() )
				return NULL;
			return iter->second;
		}
		
		
		/**
		 Function to easily get value for a key creating if necessary
		 */

		V* findOrCreate( K& key )
		{
			typename std::map<K, V* >::iterator iter = find(key);
			
			if( iter == std::map<K,V*>::end() )
			{
				// Create and insert for this key
				V* value = new V();
				insert( std::pair<K,V*>( key, value) );
				return value;
			}
			return iter->second;
			
		}

		/** 
		 Function to remove a particular entry if exist
		 Return if it really existed
		 */
		
		bool removeInMap( K key ) 
		{
			typename std::map<K, V* >::iterator iter = std::map<K,V*>::find(key);
			
			if( iter == std::map<K,V*>::end() )
				return false;
			else
			{
				delete iter->second;
				std::map<K,V*>::erase( iter );
				return true;
			}
		}
		
		V* extractFromMap(  K key )
		{
			typename std::map<K, V* >::iterator iter = std::map<K,V*>::find(key);
			
			if( iter == std::map<K,V*>::end() )
				return NULL;
			else
			{
				V* v = iter->second;
				std::map<K,V*>::erase(iter);
				return v;
			}
			
		}		
	
		void clearMap()
		{
			typename std::map<K, V* >::iterator iter;

			for (iter = std::map<K,V*>::begin() ; iter != std::map<K,V*>::end() ; iter++)
			{
				delete iter->second;
			}
			std::map<K, V* >::clear();
		}
		
	};

	
	/**
	 ListMap is a container that can be used as map and as list.
	 It allows to create an associative container ( map ) but can be access to the front or back elements
	 
	 Now it is implemented with standart library elements but it should be implemented as a map to
	 an intermediate class that is itself a double linked list.
	 */
	
	template <typename K,class V>
	class ListMap
	{
		
	public:
		
		std::map<K,V*> map;	// Real associate map of elements
		std::list<K> keys;	// List of the keys to access front and back elements
		
		
		// Iterator definition
		typename std::map<K, V* >::iterator iter;
		
		
		V* insertAtFront( K& key , V* value)
		{
			V* tmp =  extractFromMap( key );
			map.insert( std::pair<K,V*>( key, value) );
			
			// Insert the key in the list
			keys.push_front( key );
			
			return tmp;
		}

		V* insertAtBack( K& key , V* value)
		{
			V* tmp =  extractFromMap( key );
			map.insert( std::pair<K,V*>( key, value) );
			
			// Insert the key in the list
			keys.push_back( key );
			
			return tmp;
		}
		
		V* extractFromBack( )
		{
			
			if( keys.size() == 0)
				return NULL;
			
			// Get the key from the back position
			K key = keys.back();
			keys.pop_back();
			
			typename std::map<K, V* >::iterator iter = map.find(key);
			
			if( iter == map.end() )
				return NULL;
			else
			{
				V* v = iter->second;
				map.erase(iter);
				keys.remove( key );
				return v;
			}
			
		}				

		V* extractFromFront( )
		{
			if( keys.size() == 0)
				return NULL;
			
			
			// Get the key from the back position
			K key = keys.front();
			keys.pop_front();
			
			typename std::map<K, V* >::iterator iter = map.find(key);
			
			if( iter == map.end() )
				return NULL;
			else
			{
				V* v = iter->second;
				map.erase(iter);
				keys.remove( key );
				return v;
			}
			
		}				
		
		/*
		 Function to easyly get pointers in a std::map < value , Pointer* >
		 NULL if not found
		 */
		
		V* findInMap( K& key ) 
		{
			typename std::map<K, V* >::iterator iter = map.find(key);
			
			if( iter == map.end() )
				return NULL;
			return iter->second;
		}
		
		/** 
		 Function to remove a particular entry if exist
		 Return if it really existed
		 */
		
		V* extractFromMap( K& key )
		{
			typename std::map<K, V* >::iterator iter = map.find(key);
			
			if( iter == map.end() )
				return NULL;
			else
			{
				V* v = iter->second;
				map.erase(iter);
				keys.remove( key );
				return v;
			}
		}		
		
		size_t size()
		{
			return map.size();
		}
		
	};	

	/**
	 Class to use map structures of <typedef,typedef> with addittional function for easy manitpulation
	 */
	
	template <typename K,class V>
	class simple_map : public std::map<K,V>
	{
		
	public:
		
		// Iterator definition
		typename std::map<K, V >::iterator iter;
		
		
		// Insert a pair of elements ( easy method )
		// Returns the previous elements if any
		void insertInMap( K& key , V& value)
		{
			removeInMap( key );
			insert( std::pair<K,V>( key, value) );
		}
		

		bool isInMap( K& key ) 
		{
			typename std::map<K, V >::iterator iter = find(key);
			return( iter != std::map<K,V>::end() );
		}
		
		/*
		 Function to easyly get pointers in a std::map < value , Pointer* >
		 NULL if not found
		 */
		
		V findInMap( K& key ) 
		{
			typename std::map<K, V >::iterator iter = find(key);
			typename std::map<K, V >::iterator iter_end = std::map<K, V >::end();
			
			if ( iter == iter_end )
			  LM_X(1,("Error using findInMap. Please check first with isInMap"));

			return iter->second;
		}
		
		
		/** 
		 Function to remove a particular entry if exist
		 Return if it really existed
		 */
		
		bool removeInMap( K& key ) 
		{
			typename std::map<K, V >::iterator iter = std::map<K,V>::find(key);
			
			if( iter == std::map<K,V>::end() )
				return false;
			else
			{
				std::map<K,V>::erase( iter );
				return true;
			}
		}
		
		V extractFromMap(  K& key )
		{
			typename std::map<K, V >::iterator iter = std::map<K,V>::find(key);
			typename std::map<K, V >::iterator iter_end = std::map<K,V>::end();
			
			if( iter == iter_end )
			{
				// Make sure to call isInMap before
				LM_X(1,("Error extracting an element from an au::simple_map without checking first it was included"));
			}
			
			V v = iter->second;
			std::map<K,V>::erase(iter);
			return v;
			
		}		
		
	};
	

		
  
}

#endif

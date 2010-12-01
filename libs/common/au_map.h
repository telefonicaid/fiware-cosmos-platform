#ifndef _H_AU_MAP
#define _H_AU_MAP

#include <map>		// std::map
#include <vector>	// std::vector
#include <assert.h>	// assert(.)

namespace au {

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
		// Returns the previous elements if any
		V* insertInMap( K& key , V* value)
		{
			V* tmp =  extractFromMap( key );
			insert( std::pair<K,V*>( key, value) );
			return tmp;
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
			
			assert( iter != iter_end );
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
			
			assert( iter != iter_end );	// Make sure to call isInMap before
			
			V v = iter->second;
			std::map<K,V>::erase(iter);
			return v;
			
		}		
		
	};
	
	
	
	/**
	 Function to easyly insert elements in a std::map < value , Pointer* >
	*/

	template <typename T,class T2>
	void insertInMap( std::map<T,T2*> &map , T key , T2* value )
	{
	  map.insert( std::pair<T,T2*>( key, value) );
	}

	/**
	 Function to easyly get pointers in a std::map < value , Pointer* >
	 NULL if not found
	*/

	template<typename K, class V> 
	V* findInMap(  std::map<K,V*> &m , K key ) 
	{
	  typename std::map<K, V* >::iterator iter = m.find(key);

	  if( iter == m.end() )
	    return NULL;
	  return iter->second;
	}

	/** 
	  Function to remove a particular entry if exist
	  Return if it really existed
	*/

	template<typename K, class V> 
	bool removeInMap(  std::map<K,V*> &m , K key ) 
	{
	  typename std::map<K, V* >::iterator iter = m.find(key);

	  if( iter == m.end() )
	    return false;
	  else
	    {
	      m.erase( iter );
	      return true;
	  }
	}
	
	template<typename K, class V> 
	V* extractFromMap(  std::map<K,V*> &m , K key )
	{
		typename std::map<K, V* >::iterator iter = m.find(key);
		
		if( iter == m.end() )
			return NULL;
		else
		{
			V* v = iter->second;
			m.erase(iter);
			return v;
		}
		
	}

		
  
}

#endif

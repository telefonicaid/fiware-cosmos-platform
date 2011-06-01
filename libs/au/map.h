#ifndef _H_AU_MAP
#define _H_AU_MAP

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set
#include <string.h>
#include <iostream>

#include "logMsg/logMsg.h"					 // LM_M()

namespace au {

    /**
        Class to use map structures of <typedef,class*> with addittional function for easy manitpulation
	 */

    template <typename K, typename V, typename _Compare = std::less<K> >
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
            V* tmp = findInMap(key);
            if( !tmp )
            {
                tmp = new V();
                insertInMap(key, tmp);
            }
            return tmp;			
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
	
    
    // Spetial map with const char* as key
    
    struct strCompare : public std::binary_function<const char*, const char*, bool> {
    public:
        bool operator() (const char* str1, const char* str2) const
        { return strcmp(str1, str2) < 0; }
    };

    

    

    
    
  
}

#endif

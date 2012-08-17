
/* ****************************************************************************
 *
 * FILE            simple_map
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Wrapper of the std::map object with convenient methods for easy development
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_SIMPLE_MAP
#define _H_AU_SIMPLE_MAP

#include <map>

#include "logMsg/logMsg.h"      // LM_X




namespace au {

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
    void insertInMap( const K& key , const V& value)
    {
        removeInMap( key );
        std::map<K, V >::insert( std::pair<K,V>( key, value) );
    }
    
    
    bool isInMap( const K& key ) 
    {
        typename std::map<K, V >::iterator iter = std::map<K, V >::find(key);
        return( iter != std::map<K,V>::end() );
    }
    
    /*
     Function to easyly get pointers in a std::map < value , Pointer* >
     NULL if not found
     */
    
    V findInMap( const K& key ) 
    {
        typename std::map<K, V >::iterator iter = std::map<K, V >::find(key);
        typename std::map<K, V >::iterator iter_end = std::map<K, V >::end();
        
        if ( iter == iter_end )
            LM_X(1,("Error using findInMap. Please check first with isInMap"));
        
        return iter->second;
    }
    
    
    /** 
     Function to remove a particular entry if exist
     Return if it really existed
     */
    
    bool removeInMap( const K& key ) 
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
    
    V extractFromMap(  const K& key )
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


/* ****************************************************************************
 *
 * FILE            list
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      Specialized version of the std::list when included objects are pointers.
 *      In this case, we can return NULL if object is not found in the list
 *      It greatly simplifies development of objects managers in memory
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_VECTOR
#define _H_AU_VECTOR

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set

#include "logMsg/logMsg.h"					 // LM_M()

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

/**
 CLass to use list structures of <class*> with additional function for easy manipulation
 */

template <class V>
class vector : public std::vector<V*>
{
public:
    
    void clearVector()
    {
        for ( size_t i = 0 ; i < std::vector<V*>::size() ; i++ )
            delete (*this)[i];
        std::vector<V*>::clear();
    }
    
    bool removeFromVector( V* v )
    {
        bool result = false;
        
        typename std::vector<V*>::iterator it;
        
        for ( it = std::vector<V*>::begin() ; it != std::vector<V*>::end() ; )
        {
            if( *it == v )
            {
                it = std::vector<V*>::erase( it );
                result = true;
            }
            else
                it++;
        }
        
        return result;
    }
    
    
    void reverse()
    {
        std::vector<V*> tmp;
        size_t num = std::vector<V*>::size();
        for( size_t i = 0 ; i < num ; i++ )
            tmp.push_back( (*this)[num-i-1] );
        std::vector<V*>::clear();
        for( size_t i = 0 ; i < num ; i++ )
            std::vector<V*>::push_back( tmp[i] );
        
    }
    
};	

NAMESPACE_END

#endif


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

#ifndef _H_AU_LIST
#define _H_AU_LIST

#include <map>		// std::map
#include <list>		// std::list
#include <vector>	// std::vector
#include <set>		// std::set

#include "logMsg/logMsg.h"					 // LM_M()




namespace au {

/**
 CLass to use list structures of <class*> with additional function for easy manipulation
 */

template <class V>
class list: public std::list<V*>
{
public:
    
    V* findFront()
    {
        if( std::list<V*>::size() == 0)
            return NULL;
        else
        {
            V* tmp = std::list<V*>::front();
            return tmp;
        }
    }
    
    V* findBack()
    {
        if( std::list<V*>::size() == 0)
            return NULL;
        else
        {
            V* tmp = std::list<V*>::back();
            return tmp;
        }
    }
    
    
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
    
    V* extractFromList(  V* v )
    {
        typename std::list<V*>::iterator iter;
        
        for (iter = std::list<V*>::begin() ;  iter != std::list<V*>::end() ; iter++ )
            if( *iter == v )
                break;
        
        if( iter == std::list<V*>::end() )
            return NULL;
        else
        {
            std::list<V*>::erase(iter);
            return v;
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

    void releaseList()
    {
        typename std::list<V* >::iterator iter;
        
        for (iter =  std::list<V*>::begin() ; iter != std::list<V*>::end() ; iter++)
        {
            (*iter)->Release();
        }
        std::list<V*>::clear();
    }
    
    
};	

}

#endif

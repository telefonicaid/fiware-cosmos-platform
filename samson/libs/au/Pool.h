
/* ****************************************************************************
 *
 * FILE            Pool
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            2012
 *
 * DESCRIPTION
 *
 *  Class used as a a pool of instances of particular class
 *
 * COPYRIGTH       Copyright 2012 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/


#ifndef _H_AU_POOL
#define _H_AU_POOL

#include <cstring>
#include <string>    // std::String
#include <time.h>
#include <list>
#include <math.h>

#include "au/xml.h"
#include "au/Cronometer.h"
#include "au/string.h"

#include "au/namespace.h"


namespace au 
{
    template <class C>
    class Pool
    {
        std::vector<C*> objects;
        
    public:
        
        C* pop()
        {
            size_t s = objects.size();
            
            if( s == 0 )
                return NULL;
            
            C* c = objects[ s-1 ];
            objects.pop_back();
            return c;
        }
        
        void push( C*c )
        {
            objects.push_back(c);
            
        }
        
        size_t size()
        {
            return objects.size();
        }
        
    };
    
    
}

#endif


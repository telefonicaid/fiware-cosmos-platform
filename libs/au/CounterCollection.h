
/* ****************************************************************************
 *
 * FILE            -
 *
 * AUTHOR          Andreu Urruela
 *
 * PROJECT         au library
 *
 * DATE            Septembre 2011
 *
 * DESCRIPTION
 *
 *      CounterCollection is a class used to keep an arbitrary number of counters by a particular "key"
 *      For instance could be a string, so we keep a coutner for each string.
 *
 * COPYRIGTH       Copyright 2011 Andreu Urruela. All rights reserved.
 *
 * ****************************************************************************/

#ifndef _H_AU_COUNTER_COLLECTION
#define _H_AU_COUNTER_COLLECTION

#include <string>
#include <sstream>
#include "au/map.h"       // au::map

#include "au/au_namespace.h"


NAMESPACE_BEGIN(au)

class Counter
{
    int c;
    
public:
    
    Counter();
    
    int get();
};

template <typename T>
class CounterCollection
{
    au::map<T,Counter> counters;
    
public:
    
    int getCounterFor(T t)
    {
        Counter *c = counters.findInMap( t );
        if( !c )
        {
            c = new Counter();
            counters.insertInMap( t , c);
        }
        return c->get();
    }
};

NAMESPACE_END

#endif
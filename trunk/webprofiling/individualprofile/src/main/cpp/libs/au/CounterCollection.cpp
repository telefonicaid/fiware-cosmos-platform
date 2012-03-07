

#include "CounterCollection.h"      // Own interface

NAMESPACE_BEGIN(au)

Counter::Counter()
{
    c = 0;
}

int Counter::get()
{
    return c++;
}

NAMESPACE_END  

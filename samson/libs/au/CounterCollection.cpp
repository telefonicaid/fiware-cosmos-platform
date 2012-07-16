

#include "CounterCollection.h"      // Own interface

NAMESPACE_BEGIN(au)

Counter::Counter()
{
    c = 0;
}

int Counter::get()
{
    return c;
}
void Counter::append( int v )
{
    c += v;
}

NAMESPACE_END  

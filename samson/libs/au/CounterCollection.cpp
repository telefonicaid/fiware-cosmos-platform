

#include "CounterCollection.h"      // Own interface

namespace au {

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

}  

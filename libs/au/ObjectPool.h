
#ifndef _AU_OBJECT_POOL
#define _AU_OBJECT_POOL

#include "au/namespace.h"
#include "au/Token.h"
#include "au/TokenTaker.h"
#include "au/ObjectVector.h"

NAMESPACE_BEGIN(au)

class ObjectPool
{

    ObjectVector* vector;
    au::Token token;
    
public:

    ObjectPool() : token("ObjectPool")
    {
        vector = new ObjectVector();
    }
    
    ~ObjectPool()
    {
        LM_M(("Destroying pool of objects with %d elements" , (int) vector->size()));
        vector->release();
    }
    
    void add( Object* object )
    {
        // Mutex protection
        TokenTaker tt(&token);

        vector->push_back( object );
    }
    
    void clear()
    {
        // Mutex protection
        TokenTaker tt(&token);
        
        vector->clear();
    }
    
};


NAMESPACE_END

#endif

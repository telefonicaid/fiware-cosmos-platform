
#ifndef _AU_OBJECT_VECTOR
#define _AU_OBJECT_VECTOR

#include <vector>

#include "au/Object.h"
#include "au/namespace.h"

NAMESPACE_BEGIN(au)

typedef std::vector< Object* > vector_objects;

class ObjectVector : protected vector_objects , public Object
{
    
public:
    
    ~ObjectVector()
    {
        // Make sure we release all inlcuded objects
        clear();
    }
    
    void push_back( Object* object )
    {
        object->retain();
        vector_objects::push_back( object );
    }
    
    Object* operator[] (unsigned i)
    {
        if( i >= vector_objects::size() )
            return NULL;
        
        return vector_objects::at(i);
    }

    Object* at(unsigned i)
    {
        if( i >= vector_objects::size() )
            return NULL;
        
        return vector_objects::at(i);
    }
    
    
    Object* extractFromBack()
    {
        if( vector_objects::size() == 0 )
            return NULL;
        
        Object* object = vector_objects::back();
        vector_objects::pop_back();
        
        // Auto release is necessary because maybe we are the last owners..
        return  object->auto_release();
    }
    
    void clear()
    {
        for ( size_t i=0 ; i < vector_objects::size() ; i++ )
        {
            Object *object = vector_objects::at(i);
            object->release();
        }
        
        // Remove all elements
        vector_objects::clear();
    }
    
    size_t size()
    {
        return vector_objects::size();
    }
    
    
};


NAMESPACE_END

#endif



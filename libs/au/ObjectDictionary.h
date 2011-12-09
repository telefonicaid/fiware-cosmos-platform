
#ifndef _AU_OBJECT_DICTIONARY
#define _AU_OBJECT_DICTIONARY

#include "au/namespace.h"
#include "au/Object.h"
#include "au/map.h"

NAMESPACE_BEGIN(au)

typedef au::map< std::string , Object > map_objects;
typedef au::map< std::string , Object >::iterator map_objects_iterator;

class ObjectDictionary : protected map_objects , public Object
{
    
public:
    
    ~ObjectDictionary()
    {
        // Make sure we release all inlcuded objects
        clear();
    }
    
    Object* findInMap( std::string name )
    {
        return map_objects::findInMap(name);
    }
    
    void insertInMap( std::string name  ,  Object* object )
    {
        // Retain the new object ( it will be introducted in the vector )
        // Note: this has to be done before running extractFromMap ( it can be the same element )
        object->retain();
        
        // Make sure we remove nicely a previously introducted elemnt with the name name
        extractFromMap(name);
        
        map_objects::insertNewInMap(name, object);
    }

    Object* extractFromMap( std::string name )
    {
        Object* object = map_objects::extractFromMap(name);

        // Object should be auto_released because we can be the last owner
        if( object )
            object->auto_release();
        
        return object;
    }

    void clear()
    {
        for( map_objects_iterator it = map_objects::begin() ; it != map_objects::end() ; it++ )
        {
            Object* object = it->second;
            object->release();
        }
        map_objects::clear();
        
    }
    
    size_t size()
    {
        return map_objects::size();
    }
    
    
};

NAMESPACE_END

#endif


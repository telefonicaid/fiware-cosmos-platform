
#ifndef _H_MESSAGES_OPERATIONS
#define _H_MESSAGES_OPERATIONS

#include <fnmatch.h>

#include "au/containers/map.h"

#include "samson/common/samson.pb.h"	// network:...

#include "samson/module/KVFormat.h"     // KVFormat
#include "samson/common/samson.pb.h"    // network::

namespace samson {
    
	bool filterName( const std::string& name , const std::string& begin , const std::string& end);
    
    template <typename C>
    void add( samson::network::CollectionRecord* record , std::string name , C _value , std::string format )
    {
        std::ostringstream value;
        value << _value;
        
        samson::network::CollectionItem *item = record->add_item();
        item->set_name(name);
        item->set_value(value.str());
        item->set_format(format);
    }
    
    template <typename C>
    void add( samson::network::CollectionRecord* record , std::string name , C _value )
    {
        std::ostringstream value;
        value << _value;
        
        samson::network::CollectionItem *item = record->add_item();
        item->set_name(name);
        item->set_value(value.str());
    }
    
    template <typename C>
    bool name_match( const char * pattern , C _value )
    {
        std::ostringstream value;
        value << _value;
        
        return ( ::fnmatch( pattern  , value.str().c_str() , 0 ) == 0);
    }
	
}

#endif

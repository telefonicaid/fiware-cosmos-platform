

#include "BlockInfo.h"      // Own interface


namespace samson
{
    
    std::string BlockInfo::strShort()
    {
        if (num_blocks == 0 )
            return "empty";
        
        return au::str( "%s %s %s %c%c" 
                       , au::str( num_blocks , "b").c_str() 
                       , au::str( info.kvs , "kvs" ).c_str()
                       , au::str( info.size ,"bytes" ).c_str()
                       , ( size_on_memory == size )?"M":" "
                       , ( size_on_disk == size )?"D":" "
                       );
    }        

    
}
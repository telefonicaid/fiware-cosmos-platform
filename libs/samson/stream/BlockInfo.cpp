

#include "BlockInfo.h"      // Own interface


namespace samson
{
    
    std::string BlockInfo::strShort()
    {
        if (num_blocks == 0 )
            return "empty";
        
        return au::str( "%3d %s %s %c%c" 
                       , num_blocks 
                       , au::str( info.kvs , "kvs" ).c_str()
                       , au::str( info.size ,"B" ).c_str()
                       , ( size_on_memory == size )?'M':' '
                       , ( size_on_disk == size )?'D':' '
                       );
    }        

    
}
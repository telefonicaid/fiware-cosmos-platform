

#include "BlockInfo.h"      // Own interface


namespace samson
{
    
    std::string BlockInfo::strShort()
    {
        if (num_blocks == 0 )
            return "empty";
        
        return au::str( "%s %s" 
                       , au::str( num_blocks , "Blocs").c_str() 
                       , info.str().c_str()
                       );
    }        

    
}
#include "samson/common/coding.h"

namespace samson
{

    bool operator==(const KVFormat & left, const KVFormat & right)
    {
        if ( left.keyFormat != right.keyFormat )
            return false;
        if ( left.valueFormat != right.valueFormat )
            return false;
        return true;
    }

    bool operator!=(const KVFormat & left, const KVFormat & right)
    {
        if ( left.keyFormat != right.keyFormat )
            return true;
        if ( left.valueFormat != right.valueFormat )
            return true;
        return false;
    }
	
    
    
    bool operator<(const KVRange & left, const KVRange & right)
    {
        if( left.hg_begin < left.hg_begin )
            return true;
        if( left.hg_begin > left.hg_begin )
            return false;
        
        if( left.hg_end < left.hg_end )
            return true;
        
        return false;
    }    

    bool operator!=(const KVRange & left, const KVRange & right)
    {
        if( left.hg_begin != right.hg_begin )
            return true;
        
        if( left.hg_end != right.hg_end )
            return true;
        
        return false;
    }

    bool operator==(const KVRange & left, const KVRange & right)
    {
        if( left.hg_begin != right.hg_begin )
            return false;
        
        if( left.hg_end != right.hg_end )
            return false;
        
        return true;
        
    }
    
}

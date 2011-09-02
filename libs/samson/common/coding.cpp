#include "samson/common/coding.h"

namespace samson
{

	
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

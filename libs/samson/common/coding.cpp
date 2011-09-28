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
    
    bool KVRange::isValidForNumDivisions( int num_divisions )
    {
        int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;

        if( ( hg_end - hg_begin ) > size_per_division )
            return false;
        
        int max_hg_end = ( hg_begin / size_per_division ) * size_per_division + size_per_division;

        if ( hg_end > max_hg_end )
            return false;
        
        return true;
        
    }
    
    int KVRange::getMaxNumDivisions()
    {
        int num_divisions = 1;
        
        while( true )
        {
            if( isValidForNumDivisions( num_divisions*2 ) )
                num_divisions*=2;
            else
                return num_divisions;
        }
    }
    
    // Get the division for a particular hg
    int divisionForHG( int hg , int num_divisions )
    {
        int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;
        return hg / size_per_division;
    }
 
    KVRange rangeForDivision( int pos , int num_divisions )
    {
        int size_per_division = KVFILE_NUM_HASHGROUPS / num_divisions;
        int hg_begin = pos*size_per_division;
        int hg_end = (pos+1)*size_per_division;
        
        if( pos == (num_divisions-1) )
            hg_end = KVFILE_NUM_HASHGROUPS;
        
        return KVRange( hg_begin , hg_end );
    }
    
    void clear( KVInfo* info )
    {
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++ )
            info[i].clear();
    }
    
    // Get the agregation of
    KVInfo selectRange( KVInfo* info , KVRange range )
    {
        KVInfo tmp;
        tmp.clear();
        
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
            if( range.contains(i) )
                tmp.append( info[i] );
        
        return tmp;
    }    
    
}

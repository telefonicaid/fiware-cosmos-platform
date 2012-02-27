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
    
#pragma mark KVRange
    
    KVRange::KVRange()
    {
        hg_begin = 0;
        hg_end = KVFILE_NUM_HASHGROUPS;
    }
    
    KVRange::KVRange( int _hg_begin , int _hg_end )
    {
        hg_begin = _hg_begin;
        hg_end = _hg_end;
    }
    
    void KVRange::set( int _hg_begin , int _hg_end )
    {
        hg_begin = _hg_begin;
        hg_end = _hg_end;
    }
    
    void KVRange::setFrom( KVInfo *info )
    {
        int i = 0 ;
        while( (info[i].size == 0) && ( i < KVFILE_NUM_HASHGROUPS ) )
            i++;
        
        if( i == KVFILE_NUM_HASHGROUPS)
        {
            LM_W(("No content for generated block"));
            set(0,1);   // Smaller set
            return;
        }
        
        hg_begin = i;
        
        int j = KVFILE_NUM_HASHGROUPS;
        while( (info[j-1].size == 0) && ( j > (i+1) ) )
            j--;
        
        hg_end = j;
        
        
		//LM_M(("KVRange for output block %s" , str().c_str()));
    }
    
    bool KVRange::isValid()
    {
        if ( ( hg_begin < 0 ) || (hg_begin > (KVFILE_NUM_HASHGROUPS) ) )
            return false;
        if ( ( hg_end < 0 ) || (hg_end > KVFILE_NUM_HASHGROUPS ) )
            return false;
        
        if( hg_begin >= hg_end )
            return false;
        
        return true;
    }
    
    void KVRange::getInfo( std::ostringstream& output)
    {
        au::xml_open(output, "kv_range");
        au::xml_simple( output , "hg_begin" , hg_begin );
        au::xml_simple( output , "hg_end" , hg_end );
        au::xml_close(output, "kv_range");
    }
    
    std::string KVRange::str()
    {
        return au::str("[%d %d]", hg_begin , hg_end);
    }
    
    bool KVRange::overlap( KVRange range )
    {
        if( range.hg_end <= hg_begin )
            return false;
        
        if( range.hg_begin >= hg_end )
            return false;
        
        return true;
    }
    
    int KVRange::getNumHashGroups()
    {
        return hg_end - hg_begin;
    }
    
    bool KVRange::includes( KVRange range )
    {
        if( range.hg_begin < hg_begin )
            return false;
        if( range.hg_end > hg_end )
            return false;
        
        return true;
    }
    
    bool KVRange::contains( int hg )
    {
        if( hg < hg_begin )
            return false;
        if( hg >= hg_end )
            return false;
        
        return true;
    }
    
    bool KVRange::contains(  KVRange range )
    {
        if ( range.hg_begin < hg_begin )
            return false;
        if( range.hg_end > hg_end )
            return false;
        
        return true;
    }
  
    
    // Get the maximum division pattern for this range
    
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
    
    bool KVRange::check( KVInfo* info )
    {
        for (int i = 0 ; i < KVFILE_NUM_HASHGROUPS ; i++)
            if( !contains(i) )
                if( info[i].size != 0 )
                    return false;
        return true;
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
    
    void clearKVInfoVector( KVInfo* info )
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

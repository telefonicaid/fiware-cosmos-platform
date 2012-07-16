#ifndef _H_SAMSON_KVRANGE
#define _H_SAMSON_KVRANGE

namespace samson 
{
    struct KVInfo;
    struct FullKVInfo;
    
	/**
     KVRange keeps information about a particular range of hash-groups
	 */
	
	struct KVRange
    {
        int hg_begin;
        int hg_end;
        
        KVRange();
        KVRange( int _hg_begin , int _hg_end );
        
        void set( int _hg_begin , int _hg_end );
        
        void add( KVRange range );
        
        void setFrom( KVInfo *info );        
        void setFrom( FullKVInfo *info );
        
        bool isValid();
        
        void getInfo( std::ostringstream& output);
        
        std::string str();
        
        bool overlap( KVRange range );
        
        int getNumHashGroups();
        
        bool includes( KVRange range );
        
        bool contains( int hg );
        
        bool contains(  KVRange range );
        
        // Get the maximum division pattern for this range
        bool isValidForNumDivisions( int num_divisions );
        int getMaxNumDivisions();
        
        // Check if this range is valid for this KVInfo*
        bool check( KVInfo* info );
        
    };
    
    
    // Operations to compare ranges
    bool operator<(const KVRange & left, const KVRange & right);
    bool operator==(const KVRange & left, const KVRange & right);
    bool operator!=(const KVRange & left, const KVRange & right);
    
    
    KVRange rangeForDivision( int pos , int num_divisions );

}

#endif

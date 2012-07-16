
#include "Rate.h" // Own interface

namespace samson {

    
    Rate::Rate()
    {
    }
    
    void Rate::push( size_t kvs , size_t size)
    {
        rate_kvs.push(  kvs );
        rate_size.push( size );
    }
    
    void Rate::push( FullKVInfo info )
    {
        push( info.kvs , info.size );
    }
    
    size_t Rate::get_total_size()
    {
        return rate_size.getTotalSize();
    }
    
    size_t Rate::get_total_kvs()
    {
        return rate_kvs.getTotalSize();
    }
    
    double Rate::get_rate_size()
    {
        return rate_size.getRate();
    }
    
    double Rate::get_rate_kvs()
    {
        return rate_kvs.getRate();
    }
}
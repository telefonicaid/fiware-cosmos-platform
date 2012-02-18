
#ifndef _H_SAMSON_RATE
#define _H_SAMSON_RATE



#include "au/Rate.h"

#include "samson/common/coding.h"

namespace samson {

    class Rate
    {
        au::rate::Rate rate_kvs;    // Rate for key values
        au::rate::Rate rate_size;   // Rate for size
        
    public:
        
        
        
        Rate()
        {
            rate_kvs.setTimeLength( 60 );
            rate_size.setTimeLength( 60 );
        }
        
        void push( size_t kvs , size_t size)
        {
            rate_kvs.push(  kvs );
            rate_size.push( size );
        }
        
        void push( FullKVInfo info )
        {
            push( info.kvs , info.size );
        }
        
        size_t get_total_size()
        {
            return rate_size.getTotalSize();
        }

        size_t get_total_kvs()
        {
            return rate_kvs.getTotalSize();
        }

        size_t get_rate_size()
        {
            return rate_size.getRate();
        }
        
        size_t get_rate_kvs()
        {
            return rate_kvs.getRate();
        }
        
    };

}


#endif


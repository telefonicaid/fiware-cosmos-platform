

#include "common.h" // Own interface


namespace samson
{
    namespace comscore
    {

        bool compareOriginalDictionaryEntry(const OriginalDictionaryEntry &a, const OriginalDictionaryEntry &b)
        {
            if( a.domain != b.domain )
                return a.domain < b.domain;
            if( a.order_1 != b.order_1 )
                return a.order_1 > b.order_1;
            if( a.order_2 != b.order_2 )
                return a.order_2 > b.order_2;
            
            return 0;
        }
        
        
    }
}
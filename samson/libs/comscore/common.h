


#ifndef _H_SAMSON_COMSCORE_COMMON
#define _H_SAMSON_COMSCORE_COMMON


#include "au/string.h"
#include <string>

namespace samson
{
    namespace comscore
    {
        
        // Common identifier for the entire library
        typedef unsigned int uint;

        // ----------------------------------------------------------------
        // OriginalDictionaryEntry
        // 
        // Entry in the original dictionary file
        // ----------------------------------------------------------------
        
        typedef struct
        {
            uint id;
            std::string domain;
            std::string pre_domain_pattern;
            std::string path_pattern;
            int order_1;
            int order_2;
            
            std::string str()
            {
                return au::str("%d %s %d %d" , id , domain.c_str() , order_1 , order_2 );
            }
            
        } OriginalDictionaryEntry;

        // Comparison funciton to sort entries
        bool compareOriginalDictionaryEntry(const OriginalDictionaryEntry &a, const OriginalDictionaryEntry &b);

        
        
    }
}

#endif

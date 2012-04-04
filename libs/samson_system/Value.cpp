

#include "samson_system/Value.h" // Own interface

namespace samson 
{
    namespace system 
    {
        
        
        // Static pool for object resusage
        au::Pool<Value> samson::system::Value::pool_values;

        
        // --------------------------------------------------------
        // Constant word serialization stuff
        // --------------------------------------------------------
        
        // Constant sorted alphabetically for fast search
        const char* constant_words[] =
        {
            "log"
            "url",
            "user",
            "usr",
        };

        int internal_get_constant_word_code( const char * word , int min , int max )
        {
            if( max <= (min+1) )
                return -1;
            
            int mid = (min + max)/2;
            
            int c = strcmp(word , constant_words[mid] );
            if(  c == 0 )
                return mid;
            else if( c > 0 )
                return internal_get_constant_word_code(word, mid, max);
            else
                return internal_get_constant_word_code(word, min, mid);
        }
        
        // Find if this word is a constant word ( serialized with 2 bytes )
        // Return -1 if not found
        int get_constant_word_code( const char * word )
        {
            int max = sizeof(constant_words)/sizeof(char*);
            
            if( strcmp(word , constant_words[0] ) == 0 )
                return 0;
            if( strcmp(word , constant_words[max-1] ) == 0 )
                return max-1;
            return internal_get_constant_word_code(word, 0,max-1);
        }

        const char* get_constant_word( int c )
        {
            if( ( c < 0 ) || (c > 255))
                return "Unknown";
            return constant_words[c];
        }

        
    }
}
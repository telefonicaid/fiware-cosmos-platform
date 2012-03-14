
#include <map>
#include <string>

#ifndef _H_AU_STRING_COLLECTION
#define _H_AU_STRING_COLLECTION

namespace au {

    /*
     Compressed version of a collection of strings
     Added strings are serialized to a common vector.
     When a string is added, the position inside the global vector is returned
     Duplicated strings returns the same position
     */
    
    class StringCollection
    {

        // Global pointer with all serialized strings 
        char * v;
        // Current size of the vector
        size_t size;
        // Current malloc size of the vector
        size_t max_size;

    public:


        // Map of previously added strings to avoid duplications
        std::map<std::string,int> previous_strings;
        
        // Constructor
        StringCollection();
        
        // Destructor
        ~StringCollection();

        // Add string ( cause reallocation if required )
        int add( const char * string );

        // Read and write from disk
        size_t write( FILE* file );
        void read( FILE* file , size_t _size );
        
        // Get used size on disk/memory
        size_t getSize();
        
        // Recover string at this position ( returned when added )
        const char* get( int pos );
        
        
    };
    
}

#endif

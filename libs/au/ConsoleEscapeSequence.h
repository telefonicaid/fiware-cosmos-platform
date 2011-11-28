#ifndef _AU_CONSOLE_ESCAPE_SEQUENCE
#define _AU_CONSOLE_ESCAPE_SEQUENCE

#include <string>
#include "au/ConsoleCode.h"

namespace au {
    
    class ConsoleEscapeSequence
    {
        char sequence[128];
        int pos;
        
    public:
        
        ConsoleEscapeSequence();
        
        void init();
        void add( char c );
        
        bool isFinished();
        ConsoleCode getCode();
        
        std::string description();
        
    private:
        
        bool isSequence( int c );
        bool isSequence( int c , int c2 );
        
    };
}

#endif
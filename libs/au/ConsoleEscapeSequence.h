#ifndef _AU_CONSOLE_ESCAPE_SEQUENCE
#define _AU_CONSOLE_ESCAPE_SEQUENCE

#include <string>
#include <vector>
#include "au/string.h"
#include "au/ConsoleCode.h"

#include "au/namespace.h"


NAMESPACE_BEGIN(au)


class Sequence
{
    std::string sequence;
    ConsoleCode code;
    
public:
    
    Sequence(char c , ConsoleCode _code )
    {
        // Save sequence
        sequence = au::str("%c",c);
        code = _code;
    }
    
    Sequence(char c , char c2, ConsoleCode _code )
    {
        // Save sequence
        sequence = au::str("%c%c",c,c2);
        code = _code;
    }
    
    Sequence( const char* seq , ConsoleCode _code )
    {
        // Save sequence
        sequence = seq;
        code = _code;
    }
    
    ConsoleCode getCode( const char* s , int len )
    {
        if( (int) sequence.length() < len )
            return unknown;
        
        for (int i =0;i<len;i++)
            if ( sequence[i] != s[i] )
                return unknown;
        
        if( (int) sequence.length() > len )
            return unfinished;
        return code;
    }
    
};


class ConsoleEscapeSequence
{
    std::vector<Sequence*> sequences; // Supported sequences
    
    char sequence[128];
    int pos;
    
public:
    
    ConsoleEscapeSequence();
    
    void init();
    void add( char c );
    
    ConsoleCode getCode();
    std::string description();
    
};

NAMESPACE_END

#endif
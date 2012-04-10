
#ifndef _AU_CONSOLE_ENTRY
#define _AU_CONSOLE_ENTRY

#include <string>
#include <list>
#include <termios.h>                // termios

#include "au/console/ConsoleCode.h"
#include "au/mutex/Token.h"
#include "au/namespace.h"

#include "ConsoleEscapeSequence.h"

namespace au {

    // Class used to get something from the entry
    class ConsoleEntry
    {
        
        typedef enum
        {
            normal_char,
            escape_sequence,
            unknown_escape_sequence
        } Type;
        
        Type type;
        char c;              // Entry character ( except if 0 )
        std::string seq;     // Escape sequence
        
    public:
        
        
        void setChar( char _c )
        {
            type = normal_char;
            c = _c;
        }
        
        void setEscapeSequence( std::string _seq )
        {
            type = escape_sequence;
            seq = _seq;
        }
        
        void setUnknownEscapeSequence( std::string _seq )
        {
            type = unknown_escape_sequence;
            seq = _seq;
        }
        
        bool isChar()
        {
            return ( type == normal_char);
        }
        
        bool isChar( char _c )
        {
            if ( type == normal_char)
                return (c == _c );
            else
                return false;    
        }
        
        char getChar()
        {
            if( type != normal_char )
                LM_X(1, ("Getting a char from a non char Console Entry"));
            return c;
        }
        
        bool isEscapeSequence()
        {
            return ( type == escape_sequence);
        }
        
        bool isUnknownEscapeSequence()
        {
            return ( type == unknown_escape_sequence);
        }
        
        std::string getEscapeSequece()
        {
            if( type == normal_char )
                LM_X(1, ("Getting a escape sequence from a wrong Console Entry"));
            
            return seq;
        }
        
        bool isCursorUp()
        {
            if( type != escape_sequence )
                return false;
            return ( seq == "[A" );
        }
        
        bool isCursorDown()
        {
            if( type != escape_sequence )
                return false;
            return ( seq == "[B" );
        }
        
        bool isCursorLeft()
        {
            if( type != escape_sequence )
                return false;
            return ( seq == "[D" );
        }
        
        bool isCursorRigth()
        {
            if( type != escape_sequence )
                return false;
            return ( seq == "[C" );
        }
        
    };
}

#endif


#include "au/ConsoleEscapeSequence.h" // Own interface
#include "au/string.h"

namespace au {
    
    ConsoleEscapeSequence::ConsoleEscapeSequence()
    {
        init();
    }
    
    void ConsoleEscapeSequence::init()
    {
        pos=0;
    }
    
    void ConsoleEscapeSequence::add( char c )
    {
        sequence[pos++]=c;
    }
    
    bool ConsoleEscapeSequence::isSequence( int c )
    {
        if ( pos != 1 )
            return false;
        if ( sequence[0] != c )
            return false;
        
        return true;
    }
    
    bool ConsoleEscapeSequence::isSequence( int c , int c2 )
    {
        if ( pos != 2 )
            return false;
        if ( sequence[0] != c )
            return false;
        if ( sequence[1] != c2 )
            return false;
        
        return true;
    }
    
    bool ConsoleEscapeSequence::isFinished()
    {
        if( (pos==1) && (sequence[0]==127)) // esc - del
            return true;
        
        if ( pos == 2 )
            if ( isCharInRange( sequence[1] , 64 , 95 ) )
                return true;
        
        if( pos > 2 )
            if( sequence[0] == '[' )
                if ( isCharInRange( sequence[pos-1] , 64 , 126 ) )
                    return true;
        
        return false;
        
    }
    
    std::string ConsoleEscapeSequence::description()
    {
        std::ostringstream output;
        for (int i=0 ; i<pos ; i++ )
            output << au::str("[%d]",sequence[i]);
        
        sequence[pos]='\0';
        output << au::str("(%s)",sequence);
        
        return output.str();
    }
    
    ConsoleCode ConsoleEscapeSequence::getCode()
    {
        if( (pos==1) && (sequence[0]=127))
            return del_word;
        
        if (isSequence(91,68))
            return move_backward;
        
        if (isSequence(91,67))
            return move_forward;
        
        if (isSequence(91,65))
            return move_up;
        
        if (isSequence(91,66))
            return move_down;
        
        return unknown;
    }
    
}
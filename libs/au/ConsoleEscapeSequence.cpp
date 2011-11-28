
#include "au/ConsoleEscapeSequence.h" // Own interface
#include "au/string.h"

namespace au {
    
    ConsoleEscapeSequence::ConsoleEscapeSequence()
    {
        init();
        
        // Suported sequences
        sequences.push_back( new Sequence(127 , del_word) );
        
        sequences.push_back( new Sequence(91 , 65, move_up  ) );
        sequences.push_back( new Sequence(91 , 66, move_down  ) );
        sequences.push_back( new Sequence(91 , 67, move_forward  ) );
        sequences.push_back( new Sequence(91 , 68, move_backward  ) );

        sequences.push_back( new Sequence("au", au ) );
    }
    
    void ConsoleEscapeSequence::init()
    {
        pos=0;
    }
    
    ConsoleCode ConsoleEscapeSequence::getCode()
    {
        
        for (size_t i = 0 ;  i < sequences.size() ; i++ )
        {
            ConsoleCode code = sequences[i]->getCode(sequence, pos);
            if ( code != unknown )
                return code;
        }
        
        return unknown;
        
        /*
        
        
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
         */
    }

    void ConsoleEscapeSequence::add( char c )
    {
        sequence[pos++]=c;
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
    
    
}
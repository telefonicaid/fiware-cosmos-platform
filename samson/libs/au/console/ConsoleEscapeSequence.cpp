
#include "au/console/ConsoleEscapeSequence.h" // Own interface
#include "au/string.h"

namespace au {

ConsoleEscapeSequence::ConsoleEscapeSequence()
{
    init();
    
    // Suported sequences
    addSequence(127);  // Delete
    
    addSequence( 91 , 65 ); // move_up
    addSequence( 91 , 66 ); // move_down
    addSequence( 91 , 67 ); // move_forward
    addSequence( 91 , 68 ); //move_backward
    addSequence( "au" );
    
    addSequence("h"); // History...
    
    addSequence("b"); // Block background messages
}


}
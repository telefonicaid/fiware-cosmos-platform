#ifndef _AU_CONSOLE_CODE
#define _AU_CONSOLE_CODE

#include "au/namespace.h"


NAMESPACE_BEGIN(au)

typedef enum
{
    au,         // Fancy stuff 
    tab,        // tab
    ret,        // return
    del,        // del
    del_word,   // esc del
    del_rest_line,
    move_backward,
    move_forward,
    move_home,
    move_up,
    move_down,
    move_end,
    unknown,        // Unknown
    unfinished,     // Non finished codes
} ConsoleCode;

const char* getConsoleCodeName( ConsoleCode code );

NAMESPACE_END

#endif
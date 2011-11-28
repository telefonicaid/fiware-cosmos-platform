#ifndef _AU_CONSOLE_CODE
#define _AU_CONSOLE_CODE

namespace au {

    typedef enum
    {
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
        unknown,       // Unknown
    } ConsoleCode;
    
    const char* getConsoleCodeName( ConsoleCode code );
    
}

#endif
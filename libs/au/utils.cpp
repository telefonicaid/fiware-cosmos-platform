#include <iostream>
#include <termios.h>                // termios
#include <sstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "au/string.h"

#include "au/utils.h" // Own interface

namespace au {
    
    int getColumns()
    {
        int x,y;
        get_term_size( 0 , &x, &y );

        return x;
    }
    
    void clear_line()
    {
        printf("\r");
        for (int i = 0 ;  i < getColumns() ; i++ )
            printf(" ");
        printf("\r");
        fflush(stdout);
    }

}

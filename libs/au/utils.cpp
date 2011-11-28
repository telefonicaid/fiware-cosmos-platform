#include <iostream>
#include <termios.h>                // termios
#include <sstream>
#include <sys/ioctl.h>
#include <stdio.h>
#include <signal.h>


#include "au/utils.h" // Own interface

namespace au {
    
    int getColumns()
    {
        struct ttysize ts;
        ioctl(0, TIOCGSIZE, &ts);
        
        //printf ("lines %d\n", ts.ts_lines);
        //printf ("columns %d\n", ts.ts_cols);
        
        return ts.ts_cols;
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
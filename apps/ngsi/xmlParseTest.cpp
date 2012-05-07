#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "xmlParse.h"



/* ****************************************************************************
*
* usage - 
*/
static void usage(const char* progName)
{
    printf("Usage: %s -f <input file>\n", progName);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
    char buf[4096];

    if ((argC >= 3) && (strcmp(argV[1], "-f") == 0))
    {
        int fd = open(argV[2], O_RDONLY);
        int nb;

        if (fd == -1)
        {
            printf("error opening '%s': %s\n", argV[2], strerror(errno));
            return 1;
        }

        nb = read(fd, buf, sizeof(buf));
        if (nb == -1)
        {
            printf("error reading from '%s': %s\n", argV[2], strerror(errno));
            return 2;
        }

        printf("Read %d bytes from '%s'\n", nb, argV[2]);
        buf[nb] = 0;
        xmlParse(buf);
    }
    else
    {
        usage(argV[0]);
        return 1;
    }

    return 0;
}

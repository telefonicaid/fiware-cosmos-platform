/* ****************************************************************************
*
* FILE                     sniffer.cpp
*
* DESCRIPTION              
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 16 2012
*
*
* ToDo
*
*/
#include <stdio.h>              // printf, ...
#include <sys/types.h>          // types needed by socket include files
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <errno.h>              // errno
#include <string.h>		        // strerror
#include <time.h>               // time, gmtime_r, ...
#include <sys/timeb.h>          // timeb, ftime, ...



/* ****************************************************************************
*
* Option variables
*/
char*           host       = (char*) "172.17.200.200";
unsigned short  port       = 1099;
int             verbose    = 0;
char*           filePrefix = "arcanumPackets";



/* ****************************************************************************
*
* Global variables
*/
char   buf[4 * 1024 + 1];
int    bufSize = 4 * 1024 + 1;
char*  dateString;
char   dateBuf[80];
int    dateBufLen = 80;



/* ****************************************************************************
*
* Debug macros
*/
#define M(s)                                                      \
do {                                                              \
    printf("%s[%d]: %s: ", __FILE__, __LINE__, __FUNCTION__);     \
    printf s;                                                     \
    printf("\n");                                                 \
} while (0)

#define V0(level, s)                                              \
do {                                                              \
    if (verbose >= level)                                         \
        M(s);                                                     \
} while (0)

#define V1(s) V0(1, s)
#define V2(s) V0(2, s)
#define V3(s) V0(3, s)
#define V4(s) V0(4, s)
#define V5(s) V0(5, s)
#define E(s)  M(s)

#define X(code, s)                                                \
do {                                                              \
    M(s);                                                         \
    exit(code);                                                   \
} while (0)

#define R(r, s)                                                   \
do {                                                              \
    M(s);                                                         \
    return r;                                                     \
} while (0)



/* ****************************************************************************
*
* connectToServer - 
*/
int connectToServer()
{
	struct hostent*     hp;
	struct sockaddr_in  peer;
	int                 fd;

	if (host == NULL)
		R(-1, ("no hostname given"));
	if (port == 0)
		R(-1, ("Cannot connect to '%s' - port is ZERO", host));

	if ((hp = gethostbyname(host)) == NULL)
		R(-1, ("gethostbyname(%s): %s", host, strerror(errno)));

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		R(-1, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	V3(("Connecting to %s:%d", host, port));
	int retries = 7200;
	int tri     = 0;

	while (1)
	{
		if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			++tri;
			E(("connect intent %d failed: %s", tri, strerror(errno)));
			usleep(500000);
			if (tri > retries)
			{
				close(fd);
				R(-1, ("Cannot connect to %s, port %d (even after %d retries)", host, port, retries));
			}
		}
		else
			break;
	}

	return fd;
}


/* ****************************************************************************
*
*  storageOpen - 
*/ 
int storageOpen(void)
{
    char  path[128];
    int   fd;

    sprintf(path, "/data/indigo/%s-%s.log", filePrefix, dateString);

    fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0755);
    if (fd == -1)
    {
        printf("error opening storage file '%s': %s\n", path, strerror(errno));
        exit(1);
    }

    return fd;
}



/* ****************************************************************************
*
* dateStringGet - 
*/
char* dateStringGet(void)
{
    time_t        secondsNow = time(NULL);
    struct tm     tmP;
	struct timeb  timebuffer;

    ftime(&timebuffer);
    gmtime_r(&secondsNow, &tmP);
    // strftime(dateBuf, dateBufLen, "%Y-%m-%d %H:%M", &tmP);
    strftime(dateBuf, dateBufLen, "%Y-%m-%d", &tmP);

    return dateBuf;
}



/* ****************************************************************************
*
* readFromServer - 
*/
void readFromServer(int fd)
{
	int   nb;
	int   sz;
    int   dataLen;
    int   tot;
    int   storageFd;
    int   packets = 0;
    char* dString;

    dateString = strdup(dateStringGet());
    storageFd  = storageOpen();

    V2(("Reading from server"));
	sz = bufSize;
	while (1)
	{
		memset(buf, 0, bufSize);


        //
        // Change storage file, due to date change?
        //
        dString = dateStringGet();
        if (strcmp(dString, dateString) != 0)
        {
            free(dateString);
            dateString = strdup(dString);
            
            close(storageFd);
            storageFd  = storageOpen();
        }
        
        
        //
        // Read header that contains the data length
        //
		nb = read(fd, &dataLen, sizeof(dataLen));
		if (nb == -1)
			X(1, ("error reading from socket: %s", strerror(errno)));
		else if (nb == 0)
		{
            E(("Tunnel closed the connection - reconnecting"));
            close(fd);
            fd = connectToServer();
            continue;
        }


        //
        // Read the data
        //
        dataLen = ntohl(dataLen);
        V4(("read a header - now reading %d bytes of data ...", dataLen));        
        tot = 0;
        while (tot < dataLen)
        {
           nb = read(fd, &buf[tot], dataLen - tot);
           if (nb == -1)
               X(1, ("error reading from socket: %s", strerror(errno)));
           else if (nb == 0)
               E(("Read zero bytes"));

           tot += nb;
        }

        ++packets;
        V3(("Got package %d", packets));

        nb = write(storageFd, &dataLen, sizeof(dataLen));
        if (nb != sizeof(dataLen))
           E(("Error writing dataLen to storage file"));
        nb = write(storageFd, buf,      dataLen); 
        if (nb != dataLen)
           E(("Error writing packet %d to storage file", packets));
	}
}



/* ****************************************************************************
*
* usage - 
*/
void usage(char* progName)
{
	printf("Usage:\n");
	printf("  %s -u\n", progName);
	printf("  %s [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-host (hostname)] [-port (port to connect to)] [-filePrefix (prefix)]\n", progName);
	exit(1);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
    int  fd;
    int  ix = 1;

	while (ix < argC)
	{
        if (strcmp(argV[ix], "-u") == 0)
            usage(argV[0]);
		else if (strcmp(argV[ix], "-v") == 0)
			verbose = 1;
		else if (strcmp(argV[ix], "-vv") == 0)
			verbose = 2;
		else if (strcmp(argV[ix], "-vvv") == 0)
			verbose = 3;
		else if (strcmp(argV[ix], "-vvvv") == 0)
			verbose = 4;
		else if (strcmp(argV[ix], "-vvvvv") == 0)
			verbose = 5;
        else if (strcmp(argV[ix], "-host") == 0)
        {
            host = strdup(argV[ix + 1]);
            ++ix;
        }
        else if (strcmp(argV[ix], "-port") == 0)
        {
            port = atoi(argV[ix + 1]);
            ++ix;
        }
        else if (strcmp(argV[ix], "-filePrefix") == 0)
        {
            filePrefix = strdup(argV[ix + 1]);
            ++ix;
        }
		else
		{
 			E(("%s: unrecognized option '%s'\n\n", argV[0], argV[ix]));
			usage(argV[0]);
		}

		++ix;
	}

	fd = connectToServer();
	if (fd == -1)
		X(1, ("error connecting to host '%s', port %d", host, port));

	readFromServer(fd);

	return 0;
}

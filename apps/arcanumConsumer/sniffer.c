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
char*           filePrefix = "tektronix-";
char*           timeFormat = "%Y-%m-%d";



/* ****************************************************************************
*
* Global variables
*/
char*           dateString;
char            dateBuf[80];
int             dateBufLen = 80;
char            savedBuffer[128 * 1024];
int             savedMissing      = 0;
int             savedLen          = 0;
int             storageFd         = -1;
int             nbAccumulated     = 0;
int             packets           = 0;



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
* bufPresent - 
*/
void bufPresent(char* title, char* buf, int bufLen)
{
	int ix = 0;

	if (verbose < 2)
	   return;

	printf("----- %s -----\n", title);

	while (ix < bufLen)
	{
		if (ix % 16 == 0)
			printf("%08x:  ", ix);
		printf("%02x ", buf[ix] & 0xFF);
		++ix;
		if (ix % 16 == 0)
			printf("\n");
	}

	printf("\n");
	printf("\n");	
}



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
    strftime(dateBuf, dateBufLen, timeFormat, &tmP);

    return dateBuf;
}



/* ****************************************************************************
*
* packetStore - 
*/
void packetStore(int fd, char* buf)
{
    int  nb;
    int  bufLen = ntohl(*((int*) buf));

    bufLen += 4;
    V1(("Writing %d of data to storage file", bufLen));

    nb = write(storageFd, buf, bufLen); 
    if (nb != bufLen)
        E(("Error writing packet %d to storage file (written %d bytes out of %d)", packets, nb, bufLen));
}



/* ****************************************************************************
*
* bufPush - 
*/
void bufPush(char* buf, int size)
{
    int packetLen;

    //
    // First time?
    //
    if (storageFd == -1)
    {
        dateString = strdup(dateStringGet());
        storageFd  = storageOpen();
    }



    //
    // Change storage file, due to date change?
    //
    char* dString = dateStringGet();
    if (strcmp(dString, dateString) != 0)
    {
        free(dateString);
        dateString = strdup(dString);
		
        close(storageFd);
        storageFd  = storageOpen();
    }
        


    //
    // Any data leftover from last push?
    // Add the data from buf to make it a whole packet,
    // and then forward this 'incomplete' packet first
    //
    if (savedLen != 0)
    {
        V1(("Detected a saved buf of %d bytes, adding another %d bytes to it", savedLen, savedMissing));

        if (size >= savedMissing)
        {
            int* iP = (int*) savedBuffer;
            V1(("Restoring a saved packet: 0x%08x 0x%08x 0x%08x 0x%08x", iP[0], iP[1], iP[2], iP[3]));

            memcpy(&savedBuffer[savedLen], buf, savedMissing);

            packetStore(storageFd, savedBuffer);

            buf   = &buf[savedMissing];
            size    -= savedMissing;
            savedLen = 0;
            packets  = packets + 1;
        }
        else
        {
            V1(("Buffer too small to fill an entire packet - copying a part and reading again ..."));
            memcpy(&savedBuffer[savedLen], buf, size);
            savedLen += size;
            return;
        }
    }


    //
    // Now, finally, loop over all packets in the buf and write them to file
    //
    while (size > 0)
    {
        packetLen = ntohl(*((int*) buf));
        V1(("parsed a packet of %d data length (bigendian: 0x%x)", packetLen, *((int*) buf)));

		if (packetLen > 3000) // For example ...
           X(1, ("Bad packetLen: %d (original: 0x%x, htohl: 0x%x)", packetLen, *((int*) buf), ntohl(*((int*) buf))));

        if (size >= packetLen + 4)
        {
            ++packets;
            V1(("Got package %d (grand total bytes read: %d)", packets, nbAccumulated));

            packetStore(storageFd, buf);

            buf = &buf[packetLen + 4];
            size  -= (packetLen + 4);
        }
        else
        {
            memcpy(savedBuffer, buf, size);
            savedLen       = size;
            savedMissing   = packetLen + 4 - savedLen;

            V1(("Saved packet %d of %d bytes (packetLen: %d, so %d bytes missing ...)", packets, savedLen, packetLen, savedMissing));

            int* iP = (int*) savedBuffer;
            V1(("saved: 0x%x 0x%x 0x%x 0x%x", iP[0], iP[1], iP[2], iP[3]));
            return; // read more ...
        }
    }
}



#define BUFSIZE (16 * 1024 * 1024)
char            buffer[BUFSIZE];
int             bufSize           = BUFSIZE;
/* ****************************************************************************
*
* readFromServer - 
*/
void readFromServer(int fd)
{
	int nb;

    V2(("Reading from server"));
	while (1)
	{
        // Read as much as we possibly can ...
        nb = read(fd, buffer, sizeof(buffer));
        if (nb > 0)
        {
            nbAccumulated += nb;

            V2(("Read %d bytes of data from tunnel (grand total: %d)", nb, nbAccumulated));
			bufPresent("Read from Tunnel", buffer, nb);

            bufPush(buffer, nb);
        }
        else
        {
            if (nb == -1)
                E(("Error reading from the Tunnel: %s", strerror(errno)));
            else
                E(("The Tunnel closed the connection?"));

            close(fd);
            fd       = connectToServer();
            savedLen = 0;
        }
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
	printf("  %s [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-host (hostname)] [-port (port to connect to)] [-filePrefix (prefix)] [-timeFormat (time format for storage file)]\n", progName);

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
        else if (strcmp(argV[ix], "-timeFormat") == 0)
        {
            timeFormat = strdup(argV[ix + 1]);
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

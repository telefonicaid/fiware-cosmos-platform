/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
/* ****************************************************************************
*
* FILE                     arcanum.cpp
*
* DESCRIPTION              
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 17 2012
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
#include <signal.h>             // SIGPIPE, ...
#include <errno.h>              // errno
#include <string.h>             // strerror



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
* Option variables
*/
char*           host      = (char*) "172.17.200.200";
unsigned short  port      = 1234;
int             sleepTime = 100000;
int             sleepEach = 1;
int             verbose   = 0;
char*           path      = NULL;



/* ****************************************************************************
*
* ignore - 
*/
void ignore(int sig)
{
}



struct config
{
   int signal_no;
   sigset_t signal_mask;
   sigset_t sigpipe_mask;
   int safe_unload_delay;
};

struct config config;


struct control
{
   pthread_mutex_t mutex;

   pid_t pid;
   int socket;
   pid_t my_pid;

//   struct control_buffer command;
   char *command_args;

   bool sigpipe_pending;
   bool sigpipe_unblock;

   bool need_newline;

   int save_errno;
   int cancellation;

   bool version_mismatch;
};


#if ! defined(MSG_NOSIGNAL) && ! defined(SO_NOSIGPIPE)
#include <pthread.h>
static struct control control = { PTHREAD_MUTEX_INITIALIZER, 0, -1, 0 };
#endif

/* ****************************************************************************
*
* suppress_sigpipe - 
*/
void suppress_sigpipe(int fd)
{
#if ! defined(MSG_NOSIGNAL) && ! defined(SO_NOSIGPIPE)

   /*
    We want to ignore possible SIGPIPE that we can generate on write.
    SIGPIPE is delivered *synchronously* and *only* to the thread
    doing the write.  So if it is reported as already pending (which
    means the thread blocks it), then we do nothing: if we generate
    SIGPIPE, it will be merged with the pending one (there's no
    queuing), and that suits us well.  If it is not pending, we block
    it in this thread (and we avoid changing signal action, because it
    is per-process).
   */

   sigset_t pending;
   sigpending(&pending);
   control.sigpipe_pending = sigismember(&pending, SIGPIPE);
   if (! control.sigpipe_pending)
   {
      sigset_t blocked;
      pthread_sigmask(SIG_BLOCK, &config.sigpipe_mask, &blocked);

      /* Maybe is was blocked already?  */
      control.sigpipe_unblock = ! sigismember(&blocked, SIGPIPE);
   }
#else
    signal(SIGPIPE, ignore);
#endif  /* ! defined(MSG_NOSIGNAL) && ! defined(SO_NOSIGPIPE) */

    signal(SIGPIPE, ignore);
    sigignore(SIGPIPE);
}



/* ****************************************************************************
*
* connectToServer - 
*/
int connectToServer(void)
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

    V2(("Connecting to %s:%d", host, port));
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

    suppress_sigpipe(fd);
    return fd;
}



// 15 * 4 == 60
// 60 + 4 == 64 == 4 lines ...
#define BUFSIZE 180 
/* ****************************************************************************
*
* writeToServer - 
*/
void writeToServer(int fd, char* path)
{
    char  buf[10000];
	int   bufSize       = BUFSIZE + 10;
    int   loopNo        = 1;
    int   bytesWritten  = 0;
    int   grandTotal    = 0;
    int   dataFd        = -1;

    if (path != NULL)
    {
        dataFd = open(path, O_RDONLY);
        if (dataFd == -1)
            X(1, ("error opening %s: %s", path, strerror(errno)));
    }

    while (1)
    {
        int  ix;
		int  tot;
		int* dataSizeP = (int*) buf;
		int  dataSize;
		int  totalSize;

        if (dataFd == -1)
        {
            dataSize    = bufSize - (loopNo % 10);
            totalSize   = dataSize + 4;
            *dataSizeP  = htonl((dataSize));
            for (ix = 4; ix < dataSize; ix++)
                buf[ix] = ix - 4;
        }
        else
        {
            int nb;

            dataSize  = 10000 - (loopNo % 123);
            nb = read(dataFd, buf, dataSize);
            if (nb == -1)
                X(1, ("error reading from input file '%s': %s", path, strerror(errno)));
            if (nb == 0)
                X(1, ("read ZERO bytes from input file '%s'", path));

            dataSize  = nb;
            totalSize = dataSize;
        }

		tot = 0;
		while (tot < totalSize)
		{
			int  nb;

			nb = write(fd, &buf[tot], totalSize - tot);
			grandTotal += nb;
			V2(("Written %d bytes to server (total: %d)", nb, grandTotal));
			if (nb == -1)
			{
				E(("write to tunnel: %s", strerror(errno)));
				E(("Assuming connection closed, reconnecting ..."));
				close(fd);
				fd = connectToServer();
				continue;
			}
			else if (nb == 0)
			{
				E(("written ZERO bytes to tunnel - assuming connection closed, reconnecting ..."));
				close(fd);
				fd = connectToServer();
				continue;
			}

			bytesWritten += nb;
			tot += nb;
		}

		V1(("written a packet of %d bytes", dataSize));

        if (sleepTime != 0)
        {
            if (loopNo % sleepEach == 0)
            {
                V1(("sleeping %d micros each %d loops", sleepTime, sleepEach));
                usleep(sleepTime);
            }
        }

        ++loopNo;

        if ((loopNo % 101) == 0)
            V1(("%d packets written, %d bytes in total", loopNo, bytesWritten));
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
    printf("  %s [-host (host)] [-port (port)] [-path (file to read from)] [-sleepTime (microsecs)] [-sleepEach (loops)] [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)]\n", progName);
    exit(1);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
    if (argC == 1)
    {
       usage(argV[0]);
       exit(1);
    }

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
        else if (strcmp(argV[ix], "-sleepTime") == 0)
        {
            sleepTime = atoi(argV[ix + 1]);
            M(("Sleep time: %d microseconds", sleepTime));
            ++ix;
        }
        else if (strcmp(argV[ix], "-sleepEach") == 0)
        {
            sleepEach = atoi(argV[ix + 1]);
            M(("Sleep each %d loops", sleepEach));
            ++ix;
        }
        else if (strcmp(argV[ix], "-path") == 0)
        {
            path = strdup(argV[ix + 1]);
            M(("Reading from file '%s'", path));
            ++ix;
        }
        else
        {
            E(("%s: unrecognized option '%s'\n\n", argV[0], argV[ix]));
            usage(argV[0]);
        }

        ++ix;
    }

    int fd = connectToServer();

    if (fd == -1)
        X(1, ("error connecting to host '%s', port %d", host, port));

    signal(SIGPIPE, ignore);
    sigignore(SIGPIPE);

    writeToServer(fd, path);

    return 0;
}

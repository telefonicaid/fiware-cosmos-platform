/* ****************************************************************************
*
* FILE                     arcanumConsumer.cpp
*
* DESCRIPTION              
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 9 2011
*
*
* ToDo
*
*/
#include <stdio.h>              // printf
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
#include <string.h>		// strerror



/* ****************************************************************************
*
* Option variables
*/
unsigned short  port;



/* ****************************************************************************
*
* serverInit - 
*/
int serverInit(unsigned short port)
{
	int                 reuse = 1;
	int                 fd;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		printf("socket: %s\n", strerror(errno));
		return -1;
	}

	fcntl(fd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(fd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		close(fd);
		printf("bind to port %d: %s\n", port, strerror(errno));
		return -1;
	}

	if (listen(fd, 10) == -1)
	{
		close(fd);
		printf("listen to port %d\n", port);
		return -1;
	}

	return fd;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	int             fds;
	int             fd;
	int             cFd = -1;
	fd_set          rFds;
	struct timeval  timeVal;
	int             max;

	if (argC == 2)
		port = atoi(argV[1]);
	else
		port = 1099;

	fd = serverInit(port);

	
	while (1)
	{
		timeVal.tv_sec  = 1;
		timeVal.tv_usec = 0;

		FD_ZERO(&rFds);
		FD_SET(fd, &rFds);
		max = fd;

		if (cFd != -1)
		{
			FD_SET(cFd, &rFds);
			max = (fd > cFd)? fd : cFd;
		}

		do
		{
			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if ((fds == 1) && (FD_ISSET(fd, &rFds)))
		{
			struct sockaddr_in  sin;
			unsigned int        len         = sizeof(sin);
			
			if (cFd != -1)
				close(cFd);
			cFd = accept(fd, (struct sockaddr*) &sin, &len);
			if (cFd == -1)
			{
				printf("accept: %s\n", strerror(errno));
				return -1;
			}
		}

		if (cFd != -1)
		{
            int nb;

            nb = write(cFd, "This is a line\n", 16);
        }
	}

	return 0;
}

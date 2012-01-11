/* ****************************************************************************
*
* FILE                      arcanumTunnel.c - interconnect two machines 
*
* AUTHOR                    Ken Zangelin, Telefonica I+D
*
* CREATION DATE             January 9, 2012
*
* COPYRIGHT                 Nah, not really necessary ... :-)
*
*
*/
#include <stdio.h>              // printf
#include <string.h>             // strstr, etc ...
#include <sys/types.h>          // types needed by socket include files
#include <errno.h>              // errno
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY
#include <unistd.h>             // close
#include <fcntl.h>              // fcntl, F_SETFD
#include <inttypes.h>           // int32_t, ...



/* ****************************************************************************
*
* MAX - 
*/
#ifndef MAX
#define MAX(a, b) (((a) > (b))? (a) : (b))
#endif



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
* NodeRole - server or client ...
*/
typedef enum NodeRole
{
	NrUndefined,
	NrServer = 1,
	NrClient
} NodeRole;



/* ****************************************************************************
*
* Node - 
*/
typedef struct Node
{
	NodeRole        role;
	char            host[128];
	unsigned short  port;
	int             listenFd;
	int             fd;         // only allowing ONE connection ...
} Node;



/* ****************************************************************************
*
* global variables - 
*/ 
static int  verbose = 0;
static int  filter  = 0;

Node        node1;
Node        node2;
char        buffer[8 * 1024 * 1024];



/* ****************************************************************************
*
* serverInit - 
*/
void serverInit(Node* nodeP)
{
	int                 reuse = 1;
	int                 fd;
	struct sockaddr_in  sa;
	struct hostent *    heP;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		X(13, ("socket: %s\n", strerror(errno)));

	fcntl(fd, F_SETFD, 1);

	memset((char*) &sa, 0, sizeof(sa));

	if (nodeP->host == NULL)
		sa.sin_addr.s_addr = INADDR_ANY;
	else
	{
		struct hostent* heP;
		
		heP = gethostbyname(nodeP->host);
		bcopy(heP->h_addr, &sa.sin_addr, heP->h_length);
	}

	sa.sin_family      = AF_INET;
	sa.sin_port        = htons(nodeP->port);
	
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(fd, (struct sockaddr*) &sa, sizeof(struct sockaddr_in)) == -1)
	{
		close(fd);
		X(12, ("bind to port %d: %s\n", nodeP->port, strerror(errno)));
	}

	if (listen(fd, 10) == -1)
	{
		close(fd);
		X(11, ("listen to port %d\n", nodeP->port));
	}

	nodeP->listenFd = fd;
	nodeP->fd       = -1;

	V3(("Opened listen socket on fd %d for connections from '%s', port %d", nodeP->listenFd, nodeP->host, nodeP->port));
}



/* ****************************************************************************
*
* connectToServer - 
*/
void connectToServer(Node* nodeP)
{
	struct hostent*     hp;
	struct sockaddr_in  peer;
	int                 fd;

	if ((hp = gethostbyname(nodeP->host)) == NULL)
		X(23, ("gethostbyname(%s): %s", nodeP->host, strerror(errno)));

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		X(24, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(nodeP->port);

	V2(("Connecting to %s:%d", nodeP->host, nodeP->port));
	int retries = 200;
	int tri     = 0;

	while (1)
	{
		if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			++tri;
			E(("connect intent %d failed: %s", tri, strerror(errno)));
			usleep(50000);
			if (tri > retries)
			{
				close(fd);
				X(25, ("Cannot connect to %s, port %d (even after %d retries)", nodeP->host, nodeP->port, retries));
			}
		}
		else
			break;
	}

	nodeP->fd = fd;
}



/* ****************************************************************************
*
* acceptConnection - 
*/
void acceptConnection(Node* nodeP)
{
	nodeP->fd = accept(nodeP->listenFd, NULL, 0);
	if (nodeP->fd == -1)
		X(6, ("accept: %s", strerror(errno)));
} 



/* ****************************************************************************
*
* nodeParse - 
*/
void nodeParse(char* nodeInfo, Node* nodeP)
{
	char* nodeInfoCopy = strdup(nodeInfo);
	char* portStart;
	char  role[32];

	V5(("parsing node '%s'", nodeInfo));

	char* tmP = strstr(nodeInfo, ":");
	if (tmP == NULL)
		X(2, ("No colon found in '%s' - bad format, sorry", nodeInfo));

	*tmP   = 0;
	++tmP;
	strcpy(nodeP->host, nodeInfo);
	portStart = tmP;
	
	tmP = strstr(tmP, ":");
	if (tmP == NULL)
        X(3, ("Second colon not found in '%s' - bad format, sorry", nodeInfoCopy));
	*tmP = 0;
	++tmP;
	nodeP->port = atoi(portStart);
	strcpy(role, tmP);

	if (strcmp(role, "server") == 0)
		nodeP->role = NrServer;
	else if (strcmp(role, "client") == 0)
		nodeP->role = NrClient;
	else
		X(4, ("Bad role in '%s': '%s' - must be either 'server' or 'client'", nodeInfoCopy, role));

	V4(("host: '%s', port: '%d', role: %s", nodeP->host, nodeP->port, (nodeP->role == NrClient)? "client" : "server"));
	free(nodeInfoCopy);
}



/* ****************************************************************************
*
* nodeInit - 
*/
int nodeInit(Node* nodeP)
{
	int fd;
	
	if (nodeP->role == NrServer)
	{
		V2(("Initializing server for host '%s' and port %d", nodeP->host, nodeP->port));
		serverInit(nodeP);
		V2(("Accepting connections from '%s', port %d", nodeP->host, nodeP->port));
		acceptConnection(nodeP); // Await for someone to connect
	}
	else
		connectToServer(nodeP);

	return fd;
}



/* ****************************************************************************
*
* tmoHandler - 
*/
void tmoHandler(void)
{
}



/* ****************************************************************************
*
* filteredTunnel - 
*/
void filteredTunnel(Node* from, Node* to)
{
	E(("Sorry, filter not implemented ..."));
	exit(51);
}



/* ****************************************************************************
*
* tunnel - 
*/
void tunnel(Node* from, Node* to)
{
	int nb;
	int size;
	int total;

	if (filter == 1)
	{
		filteredTunnel(from, to);
		return;
	}

	nb = read(from->fd, buffer, sizeof(buffer));
	if (nb == -1)
		X(41, ("reading data (node '%s:%d'): %s", from->host, from->port, strerror(errno)));
	else if (nb == 0)
		X(42, ("Node '%s:%d' closed the connection", from->host, from->port));
	else
	{
		size  = nb;
		total = 0;
		while (total < size)
		{
			nb = write(to->fd, &buffer[total], size - total);
			if (nb == -1)
				X(41, ("writing data to node '%s:%d': %s", to->host, to->port, strerror(errno)));
			else if (nb == 0)
				X(42, ("written ZERO bytes to node '%s:%d': %s", to->host, to->port, strerror(errno)));

			V1(("Tunneled %d bytes to '%s:%d'", nb, to->host, to->port));
			total += nb;
		}
	}
}



/* ****************************************************************************
*
* run - 
*/
void run(Node* node1, Node* node2)
{
	int             fds;
	int             max;
	fd_set          rFds;
	struct timeval  timeVal;

	V1(("tunneling between fd %d and fd %d", node1->fd, node2->fd));

	max = MAX(node1->fd, node2->fd);
	while (1)
	{
		timeVal.tv_sec  = 1;
		timeVal.tv_usec = 0;
		
		FD_ZERO(&rFds);
		FD_SET(node1->fd, &rFds);
		FD_SET(node2->fd, &rFds);

		do
		{
			fds = select(max + 1, &rFds, NULL, NULL, &timeVal);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			X(31, ("select error: %s", strerror(errno)));
		else if (fds == 0)
			tmoHandler();
		else if ((fds > 0) && (FD_ISSET(node1->fd, &rFds)))
			tunnel(node1, node2);
		else if ((fds > 0) && (FD_ISSET(node2->fd, &rFds)))
			tunnel(node2, node1);
		else
			X(32, ("What happened? select says OK (%d), but nothing to read ... ?", fds));
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
	printf("  %s ip:port:role ip:port:role [-v | -vv | -vvv | -vvvv | -vvvvv (verbose level 1-5)] [-filter (use filter to remove unwanted data)]\n", progName);
	exit(1);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	char* node1info = argV[1];
	char* node2info = argV[2];

        if (argC == 1)
        {
          usage(argV[0]);
          exit(1);
        }
	if (strcmp(argV[1], "-u") == 0)
		usage(argV[0]);

	if (argC < 3)
		usage(argV[0]);

	int ix = 3;
	while (ix < argC)
	{
		if (strcmp(argV[ix], "-v") == 0)
			verbose = 1;
		else if (strcmp(argV[ix], "-vv") == 0)
			verbose = 2;
		else if (strcmp(argV[ix], "-vvv") == 0)
			verbose = 3;
		else if (strcmp(argV[ix], "-vvvv") == 0)
			verbose = 4;
		else if (strcmp(argV[ix], "-vvvvv") == 0)
			verbose = 5;
		else if (strcmp(argV[ix], "-filter") == 0)
		{
			X(51, ("Sorry, filter not implemented ..."));
			filter = 1;
		}
		else if (strcmp(argV[ix], "-u") == 0)
			usage(argV[0]);
		else
		{
			X(1, ("%s: unrecognized option '%s'\n\nUsage: %s: IP:port:role IP2:port:role [-v <verbose level (0-5)>] [-u (usage)] [-filter (use filter to remove unwanted data)]\n",
				  argV[0], argV[ix], argV[0]));

			usage(argV[0]);
		}

		++ix;
	}


	nodeParse(node1info, &node1);
	nodeParse(node2info, &node2);

	V1(("Interconnecting '%s', port %d and '%s', port %d", node1.host, node1.port, node2.host, node2.port));

	nodeInit(&node1);
	nodeInit(&node2);

	run(&node1, &node2);

	return 0;
}

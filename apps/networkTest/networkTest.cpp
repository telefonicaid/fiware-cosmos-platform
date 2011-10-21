#include <sys/time.h>           // gettimeofday

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels
#include "parseArgs.h"          // parseArgs

#include "iomServerOpen.h"      // iomServerOpen
#include "iomAccept.h"          // iomAccept
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend
#include "iomMsgRead.h"         // iomMsgRead

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network



/* ****************************************************************************
*
* Option variables
*/
int              bufLen;
unsigned short   port;
bool             iom;
bool             gpb;
bool             server;
char             host[80];



#define LOCL (long) "localhost"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",    &port,   "PORT",    PaShortU,     PaOpt,    7777,      1024,    30000,  "port"                         },
	{ "-iom",     &iom,    "IOM",     PaBool,       PaOpt,    false,    false,     true,  "use basic IOM functions only" },
	{ "-gpb",     &gpb,    "GPB",     PaBool,       PaOpt,    false,    false,     true,  "use Google Protocol Buffers"  },
	{ "-server",  &server, "SERVER",  PaBool,       PaOpt,    false,    false,     true,  "act as server"                },
	{ "-host",    host,    "HOST",    PaStr,        PaOpt,    LOCL,     PaNL,      PaNL,  "host where server runs"       },
	{ "-bufLen",  &bufLen, "BUFLEN",  PaInt,        PaOpt,      60,        1,     10000,  "size of buffer in Megabytes"  },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



/* ****************************************************************************
*
* gettimediff - 
*/
void gettimediff(struct timeval* start, struct timeval* end, struct timeval* diff)
{
	diff->tv_sec  = end->tv_sec  - start->tv_sec;
	diff->tv_usec = end->tv_usec - start->tv_usec;

	if (diff->tv_usec < 0)
	{
		--diff->tv_sec;
		diff->tv_usec += 1000000;
	}
}



/* ****************************************************************************
*
* serverIomRun - 
*/
static void serverIomRun(int listenFd)
{
    struct timeval  tv;
    int             fds;
    fd_set          rFds;
	int             max;
	ss::Endpoint*   endpoint = new ss::Endpoint();
	
	endpoint->rFd = -1;

	LM_V(("Running IOM test as server"));

	while (1)
	{
		do
		{
			FD_ZERO(&rFds);
			
			FD_SET(listenFd, &rFds);
			max = listenFd;

			if (endpoint->rFd != -1)
			{
				FD_SET(endpoint->rFd, &rFds);
				LM_T(LmtSelect, ("select on fd %d", endpoint->rFd));
				if (endpoint->rFd > listenFd)
					max = endpoint->rFd;
			}

			tv.tv_sec  = 5;
			tv.tv_usec = 0;

			LM_D(("calling select"));
			fds = select(max + 1, &rFds, NULL, NULL, &tv);
			LM_D(("select returned %d", fds));
		} while ((fds == -1) && (errno == EINTR));
	
		if (fds == -1)
			LM_X(-1, ("select"));
		else if (fds == 0)
			LM_D(("timeout"));
		else if ((fds > 0) && (FD_ISSET(listenFd, &rFds)))
		{
			LM_T(LmtSelect, ("incoming connection ..."));
			if (endpoint->rFd != -1)
			{
				// fd already active - reject the connection
				int fd2;

				LM_V(("rejecting the connection"));
				fd2 = iomAccept(listenFd);
				if (fd2 != -1)
					close(fd2);
				LM_V(("... rejected it"));
			}
			else
			{
				LM_V(("... accepting it"));
				endpoint->rFd = iomAccept(listenFd);
			}
		}
		else if ((fds > 0) && (FD_ISSET(endpoint->rFd, &rFds)))
		{
			void*                     dataP;
			int                       dataLen;
			int                       s;
			struct timeval            start;
			struct timeval            end;
			struct timeval            diff;
			int                       microsecs;
			int                       bytes;
			ss::Message::MessageCode  msgCode;
			ss::Message::MessageType  msgType;

			LM_D(("incoming message ..."));
			s = gettimeofday(&start, NULL);
			if (s != 0)
				LM_X(1, ("gettimeofday: %s", strerror(errno)));

			// s = iomMsgRead(endpoint->rFd, (const char*) progName, &msgCode, &msgType, &dataP, &dataLen);
			if (s != 0)
			{
			   LM_W(("iomMsgRead: error %d - closing connection", s));
			   close(endpoint->rFd);
			   endpoint->rFd = -1;
			   continue;
			}

			s = gettimeofday(&end, NULL);
			if (s != 0)
				LM_X(1, ("gettimeofday: %s", strerror(errno)));

			gettimediff(&start, &end, &diff);
			microsecs = diff.tv_sec * 1000000 + diff.tv_usec;
			bytes     = dataLen + sizeof(ss::Message::Header);

			if (microsecs == 0)
				printf("Too small a packet (%d bytes) to measure throughpout\n", bytes);

			printf("read %d bytes in %d.%06d seconds - %.3f Megabytes per second\n",
				   bytes,
				   (int) diff.tv_sec,
				   (int) diff.tv_usec, 
				   ((double) bytes) / microsecs);
		}
		else
			LM_X(1, ("some unknown fd has a pending read - this is impossible !"));
	}
}		



/* ****************************************************************************
*
* clientIomRun - 
*/
static void clientIomRun(int fd, int bufLen)
{
	char*  buffer = (char*) malloc(bufLen);
	int    ix;
	int    s;

	LM_V(("Running IOM test as client"));

	if (buffer == NULL)
		LM_X(1, ("malloc(60M): %s", strerror(errno)));

	for (ix = 0; ix < bufLen; ix++)
		buffer[ix] = ix;

	while (1)
	{
		LM_T(LmtWrite, ("sending a message of %d bytes", bufLen));
		s = iomMsgSend(fd, "server", "client", ss::Message::ThroughputTest, ss::Message::Msg, buffer, bufLen);
		if (s != 0)
			LM_X(1, ("iomMsgSend error %d", s));
		sleep(4);
	}
}



/* ****************************************************************************
*
* serverGpbRun - 
*/
static void serverGpbRun(int fd)
{
}



/* ****************************************************************************
*
* serverRawRun - 
*/
static void serverRawRun(int fd)
{
}



/* ****************************************************************************
*
* clientGpbRun - 
*/
static void clientGpbRun(int fd)
{
}



/* ****************************************************************************
*
* clientRawRun - 
*/
static void clientRawRun(int fd)
{
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	int fd;

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, true);

	if (iom && gpb)
		LM_X(1, ("you cannot select both iom and gpb - please remove one of the options"));

	// Open listen socket if server, connect to the port and host if client
	if (server)
	{
		LM_T(LmtOpen, ("Opening port %d for incoming connections", port));
		fd = iomServerOpen(port);
		if (fd == -1)
			LM_X(1, ("iomServerOpen(port %d): %s", port, strerror(errno)));
	}
	else
	{
		LM_T(LmtConnect, ("Connecting to %s, port %d", host, port));
		fd = iomConnect(host, port);
		if (fd == -1)
			LM_X(1, ("iomConnect(host '%s', port %d): %s", host, port, strerror(errno)));
		LM_T(LmtConnect, ("Connected to host '%s', port %d", host, port));
	}

	LM_T(LmtOpen, ("Opened fd %d - now run!", fd));



	// Case 1 - Use only basic iom read/write functions
	// Case 2 - Use Google Protocol Buffers (GPB)
	// Case 3 - Use Network 'kz' raw data buffers
	if (iom)
	{
		if (server)
			serverIomRun(fd);
		else
			clientIomRun(fd, bufLen * 1024 * 1024);
	}
	else if (gpb)
	{
		if (server)
			serverGpbRun(fd);
		else
			clientGpbRun(fd);
	}
	else
	{
        if (server)
			serverRawRun(fd);
		else
			clientRawRun(fd);
	}

	return 0;
}
/* ****************************************************************************
*
* FILE                     main_ngsiTest.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            April 23 2012
* 
* 
*
* CALL with Juanjo 2012-04-27
* ------------------------------
*
*   1. What do we want 'finally'?
*   2. What's the minimum for end of June?
*   3. Will NECs Context Broker be used for the end of June?
*
*   IoT Broker to be used
*
*   TO IMPLEMENT: Things and Resources Management GE
*   Operations to be implemented (*mandatory):
*     * registerContext
*     * discoverContextAvailability (entityId is a regexp)
*     ? subscribeContextAvailability
*     ? notifyContextAvailability
*     ? updateContextAvailabilitySubscription
*     ? unsubscribeContextAvailability
*
* -------------------------------------------
*
* Next Steps:
*
* Fri + Mon
*   DOCUMENT ngsi-9 
*   Comments about ngsi-10
*
* Wed:
*   Conf call
*
* -------------------------------------------
*
*
*
* NGSI-9
* 
* Register/Update Context Entities, their attributes and their availability
* Query/Notification (discover) Context Entities, their attributes and their availability
* 
* FUNCTIONALITY
*   
*  Context Producers:
*    - publish data/context by 'update context' (message to Broker)
*    - update data/context
*    - send 'query context'  to Broker
*    - subscribe to Context Producer ()
* 
*  Context Broker
*    - treat 'update context'
*    - answer 'context queries'
*    - context data has an expiration time
*    - export 'register context'
*    - notify data/context to Consumers
*    - notify data/context to another Broker
*    - send 'discover context' to Applications
* 
*  Context Consumer
*    - retrieve data/context by querying the Broker
*    - subscribeContext from Broker (with duration)
*    - subscribe to Context Producer ()
* 
*  Application (is this not just a combination of Producer/Consumer?)
*    - subscribe to Context
*    - register context
*    - receive 'discover context'
* 
* Consumers must be permanently connected to the Broker in order to ne notified.
* Or should the Broker be able to connect?
* 
*
* HTTP Status Codes
*
* Informational
*   100 Continue
*   101 Switching Protocols
*   102 Processing
*   
* Success
*   200 OK
*   201 Created
*   202 Accepted
*   203 Non-Authoritative Information
*   204 No Content
*   205 Reset Content
*   206 Partial Content
*   207 Multi-Status
*   208 Already Reported
*   226 IM Used
*
* Redirection
*   300 Multiple Choices
*   301 Moved Permanently
*   302 Found
*   303 See Other
*   304 Not Modified
*   305 Use Proxy
*   306 Switch Proxy - No longer used
*   307 Temporary Redirect
*   308 Permanent Redirect
*
* Client Error
*   400 Bad Request         - request cannot be fulfilled due to bad syntax
*   402 Payment Required    - this code is not usually used
*   403 Forbidden           - legal request, but the server is refusing to respond to it
*   404 Not Found           - requested resource could not be found but may be available again in the future
*   405 Method Not Allowed  - e.g. using GET on a form which requires POST
*   406 Not Acceptable      - only capable of generating content not acceptable according to the Accept headers sent in the request
*   407 Proxy Authentication Required - The client must first authenticate itself with the proxy
*   408 Request Timeout     - 
*   409 Conflict            - 
*   410 Gone                -
*   411 Length Required     - request did not specify the length of its content, which is required by the requested resource
*   412 Precondition Failed - server does not meet preconditions that the requester put on the request
*   413 Request Entity Too Large
*   414 Request-URI Too Long
*   415 Unsupported Media Type
*   416 Requested Range Not Satisfiable
*   417 Expectation Failed
*   418 I'm a teapot
*   420 Enhance Your Calm
*   422 Unprocessable Entity
*   423 Locked               - resource that is being accessed is locked
*   424 Failed Dependency
*   425 Unordered Collection
*   426 Upgrade Required
*   428 Precondition Required
*   429 Too Many Requests
*   431 Request Header Fields Too Large
*   444 No Response
*   449 Retry With
*   450 Blocked by Windows Parental Controls
*   499 Client Closed Request
*
*
* Server Error
*   500 Internal Server Error
*   501 Not Implemented
*   502 Bad Gateway
*   503 Service Unavailable
*   504 Gateway Timeout
*   505 HTTP Version Not Supported
*   506 Variant Also Negotiates
*   507 Insufficient Storage
*   508 Loop Detected
*   509 Bandwidth Limit Exceeded
*   510 Not Extended
*   511 Network Authentication Required
*   598 Network read timeout error
*   599 Network connect timeout error
*/
#include <sys/types.h>                      // system types ...
#include <sys/socket.h>                     // socket, bind, listen
#include <sys/un.h>                         // sockaddr_un
#include <netinet/in.h>                     // struct sockaddr_in
#include <netdb.h>                          // gethostbyname
#include <arpa/inet.h>                      // inet_ntoa
#include <netinet/tcp.h>                    // TCP_NODELAY
#include <unistd.h>                         // getppid
#include <sys/stat.h>                       // umask
#include <fcntl.h>                          // fcntl
#include <string>
#include <sstream>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"                  // LM_*

#include "traceLevels.h"                    // Trace levels for log msg library
#include "database.h"                       // dbInit, dbReset
#include "httpData.h"                       // httpDataParse
#include "Attribute.h"                      // Attribute
#include "Verb.h"                           // Verb
#include "Format.h"                         // Format (XML, JSON)
#include "Entity.h"                         // Entity, entityAdd, ...
#include "Peer.h"                           // Peer
#include "rest.h"                           // restReply

using namespace std;



/* ****************************************************************************
*
* Global variables
*/
std::string version = "0.0.1";



/* ****************************************************************************
*
* Option variables
*/
int             port;
bool            fg;
bool            reset;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    { "-port",      &port,      "PORT",       PaInt,    PaOpt, 1025,     1,      9999,  "port to receive new connections" },
    { "-fg",        &fg,        "FOREGROUND", PaBool,   PaOpt, false,    false,  true,  "don't start as daemon"           },
    { "-reset",     &reset,     "RESET",      PaBool,   PaOpt, false,    false,  true,  "reset database"                  },

    PA_END_OF_ARGS
};



/* ****************************************************************************
*
* daemonize - 
*/
void daemonize(void)
{
	pid_t  pid;
	pid_t  sid;

	// already daemon
	if (getppid() == 1)
        return;

	pid = fork();
	if (pid == -1)
		LM_X(1, ("fork: %s", strerror(errno)));

	// Exiting father process
	if (pid > 0)
		exit(0);

	// Change the file mode mask */
	umask(0);

	// Removing the controlling terminal
	sid = setsid();
	if (sid == -1)
		LM_X(1, ("setsid: %s", strerror(errno)));

	// Change current working directory.
	// This prevents the current directory from being locked; hence not being able to remove it.
	if (chdir("/") == -1)
		LM_X(1, ("chdir: %s", strerror(errno)));
}



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
* ip2string - 
*/
void ip2string(int ip, char* ipString, int ipStringLen)
{
    snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
			 ip & 0xFF,
			 (ip & 0xFF00) >> 8,
			 (ip & 0xFF0000) >> 16,
			 (ip & 0xFF000000) >> 24);
}



/* ****************************************************************************
*
* run - 
*/
void run(int fd)
{
	int             fds;
	int             cFd = -1;
	fd_set          rFds;
	struct timeval  timeVal;
	int             max;

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
			char                hostName[64];
			if (cFd != -1)
				close(cFd);
			cFd = accept(fd, (struct sockaddr*) &sin, &len);
			if (cFd == -1)
				LM_X(1, ("accept: %s", strerror(errno)));

			ip2string(sin.sin_addr.s_addr, hostName, sizeof(hostName));
			peerAdd(hostName, sin.sin_addr.s_addr);
        }

		if (cFd != -1)
        {
            restServe(cFd);
            close(cFd);
            cFd = -1;
        }
    }
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
    int fd;

    paConfig("builtin prefix",                    (void*) "FW_CM_");
    paConfig("usage and exit on any warning",     (void*) true);
    paConfig("log to screen",                     (void*) true);
    paConfig("log to file",                       (void*) true);

    paConfig("default value", "-logDir",          (void*) "/tmp");
    paConfig("log file line format",              (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
    paConfig("screen line format",                (void*) "TYPE@TIME  FUNC[LINE]: TEXT");

    paParse(paArgs, argC, (char**) argV, 1, false);

    if (fg == false)
        daemonize();

    fd = serverInit(port);
    if (fd == -1)
        LM_X(1, ("error opening port %d for connections", port));

    LM_F(("Opened port %d for connections", port));


	//
	// database initialisation
	//
	if (dbInit() != 0)
		LM_X(1, ("error initializing database"));

	if (reset == true)
	{
		if (dbReset() != 0)
			LM_X(1, ("error resetting database"));
	}

	attributeInit();

    run(fd);

    return 0;
}

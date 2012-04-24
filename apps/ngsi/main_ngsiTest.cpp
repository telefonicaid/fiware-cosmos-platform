/* ****************************************************************************
*
* FILE                     main_ngsiTest.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            April 23 2012
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
*  Application (is this not jsut a combination of Producer/Consumer?)
*    - subscribe to Context
*    - register context
*    - receive 'discover context'
* 
* Consumers must be permanently connected to the Broker in order to ne notified.
* Or should the Broker be able to connect?
* 
*/
#include <sys/types.h>            // system types ...
#include <sys/socket.h>           // socket, bind, listen
#include <sys/un.h>               // sockaddr_un
#include <netinet/in.h>           // struct sockaddr_in
#include <netdb.h>                // gethostbyname
#include <arpa/inet.h>            // inet_ntoa
#include <netinet/tcp.h>          // TCP_NODELAY
#include <unistd.h>               // getppid
#include <sys/stat.h>             // umask
#include <fcntl.h>                // fcntl
#include <string>
#include <sstream>

#include "parseArgs/parseArgs.h"
#include "parseArgs/paConfig.h"
#include "logMsg/logMsg.h"

using namespace std;



/* ****************************************************************************
*
* Producer - 
*/
typedef struct Producer
{
    
} Producer;



/* ****************************************************************************
*
* Consumer - 
*/
typedef struct Consumer
{
    
} Consumer;



/* ****************************************************************************
*
* type of reply - 
*/ 
typedef enum ReplyType
{
    JSON = 1,
    XML  = 2
} ReplyType;



/* ****************************************************************************
*
* rest operation - 
*/ 
typedef enum Operation
{
    GET = 1,
    POST,
    PUT,
    DELETE
} Operation;



/* ****************************************************************************
*
* global variables
*/
char inBuf[8 * 1024];



/* ****************************************************************************
*
* Option variables
*/
int             port;
bool            fg;


/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
    { "-port",      &port,      "PORT",       PaInt,    PaOpt, 1025,     1,      9999,  "port to receive new connections" },
    { "-fg",        &fg,        "FOREGROUND", PaBool,   PaOpt, false,    false,  true,  "don't start as daemon"           },

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
* linesPresent - just for debugging ...
*/
void linesPresent(int lines, char** input)
{
    int ix;

    for (ix = 0; ix < lines; ix++)
        LM_V(("input: '%s'", input[ix]));
}



/* ****************************************************************************
*
* componentsPresent - 
*/
void componentsPresent(int components, std::string* component)
{
    int ix;

    for (ix = 0; ix < components; ix++)
        LM_V(("component[%d]: '%s'", ix, component[ix].c_str()));
}



/* ****************************************************************************
*
* lineSplit - 
*/
int lineSplit(char* buf, char** lineX)
{
    int    ix  = 0;
    char*  end = (char*) 4;  // not NULL ...
    char*  br;

    while (end != NULL)
    {
        lineX[ix] = buf;
        br        = strchr(buf, '\r');
        end       = strchr(buf, '\n');
        if (end != NULL)
        {
            *end = 0;
            buf = &end[1];
        }

        if (br != NULL)
            *br = 0;

        ++ix;
    }

    return ix;
}



/* ****************************************************************************
*
* stringSplit - 
*/
int stringSplit(char* path, std::string* component, char separator)
{
    int    ix  = 0;
    char*  end = (char*) 4;  // not NULL ...

    while (end != NULL)
    {
        end           = strchr(path, separator);
        if (end != NULL)
        {
            *end          = 0;
            component[ix] = path;
            path          = &end[1];
        }
        else
            component[ix] = path;

        ++ix;
    }

    return ix;
}



/* ****************************************************************************
*
* restReplySend - 
*/
int restReplySend(int fd, std::string data, int httpCode, ReplyType type)
{
    int                 dataLen = strlen(data.c_str());
    std::ostringstream  header;
    std::ostringstream  output;
    int                 outputLen;
    int                 nb;

    switch (httpCode)
    {
    case 200:
        header << "HTTP/1.1 200 OK\n";
        break;
        
    case 400:
        header << "HTTP/1.1 400 Bad Request\n";
        break;
        
    case 404:
        header << "HTTP/1.1 404 Not Found\n";
        break;
        
    default:
        header << "HTTP/1.1 Bad Request \n"; 
        break;
    }
    
    if (type == JSON)
        header << "Content-Type:   \"application/json; charset=utf-8\"\n";
    else
        header << "Content-Type:   \"application/xml; charset=utf-8\"\n";

    header << "Content-Length: " << dataLen << "\n";
    header << "\n";
    output << header.str() << data << "\n";

    outputLen  = strlen(output.str().c_str());
    nb         = write(fd, output.str().c_str(), outputLen);

    if (nb != outputLen)
        LM_RE(-1, ("Written only %d bytes of %d", nb, outputLen));

    return 0;
}



/* ****************************************************************************
*
* restReply - 
*/
int restReply(int fd, ReplyType type, int httpCode, const char* key, const char* value)
{
    std::ostringstream  data;

    if (type == JSON)
        data << "{" << '"' << key << '"' << " : " << '"' << value << '"' << "}";
    else
        data << "<ngsi><key>" << key << "</key>" << "<value>" << value << "</value></ngsi>";

    return restReplySend(fd, data.str(), httpCode, type);
}



/* ****************************************************************************
*
* sanityCheck - 
*/
static bool sanityCheck(const char* s)
{
    if ((s == NULL) || (s[0] == 0))
        return false;

    if (s[0] != '/')
        return false;

    if (strlen(s) < strlen("/ngsi/X"))
        return false;

    if (strncmp(s, "/ngsi/", 6) != 0)
        return false;

    return true;
}



/* ****************************************************************************
*
* restTreat - 
*/
int restTreat(Operation operation, char* path, int fd)
{
    char*        httpVersionString;
    int          httpCode = 200;
    std::string  jsonSuffix = ".json";
    std::string  xmlSuffix  = ".xml";
    ReplyType    type       = JSON;

    
    //
    // Check the input
    //
    httpVersionString = strstr(path, "HTTP/1.1");
    if (httpVersionString)
    {
        --httpVersionString;
        *httpVersionString = 0;
        ++httpVersionString;
    }
    else
    {
        httpVersionString = strstr(path, "HTTP/1.0");
        if (httpVersionString)
        {
            --httpVersionString;
            *httpVersionString = 0;
            ++httpVersionString;
        }
        else
        {
            httpVersionString  = (char*) "Unknown";
            httpCode           = 400;
        }
    }

    LM_V(("HTTP Version: '%s'", httpVersionString));
    LM_V(("path: '%s'", path));

    if (sanityCheck(path) == false)
    {
        restReply(fd, JSON, 400, "error", "Not an NGSI request");
        return -1;
    }


    //
    // Check in request ends in '.xml' or '.json'
    //
    char* suffix = &path[strlen(path) - 5];
    LM_V(("suffix: '%s'", suffix));
    if (strcmp(suffix, ".json") == 0)
    {
        LM_V(("XML reply requested"));
        type    = JSON;
        *suffix = 0;
    }
    else
    {
        ++suffix;
        LM_V(("suffix: '%s'", suffix));
        if (strcmp(suffix, ".xml") == 0)
        {
            LM_V(("XML reply requested"));
            type    = XML;
            *suffix = 0;
        }
        else
            LM_V(("Default reply is JSON"));
    }



    //
    // split path in components
    //
    std::string  component[20];
    int          components;

    components = stringSplit(&path[1], component, '/');
    componentsPresent(components, component);



    //
    // Now treat the request
    //
    if (component[0] != "NGSI10")
    {
        // This cannot happen ...  Already filtered in sanityCheck
        restReply(fd, type, 400, "error", "Not an NGSI request");
        return -1;        
    }
    
    if (component[1] == "ping")
    {
        restReply(fd, type, 200, "ping", "ok");
        return 0;
    }
    else if (component[1] == "contextEntities")
    {
        std::string entityId;
        std::string attributeName;
        std::string attributeDomainName;

        entityId = component[2];

        if (component[3] == "attributes")
        {
            attributeName = component[4];
            valueID       = component[5];

            // x = entityAttributeSet(entityId, attributeName, valueID);
            restReply(fd, type, 400, "entityAttributes", "not implemented");
        }
        else if (component[3] == "attributeDomains")
        {
            attributeDomainName = component[4];

            // x = entityAttributeDomainSet(entityId, attributeDomainName);
            restReply(fd, type, 400, "entityAttributeDomains", "not implemented");
        }
        else
            restReply(fd, type, 400, "badPathComponent3", component[3]);
    }
    else if (component[1] == "contextEntityTypes")
    {
        std::string typeName = component[2];

        if (component[3] == "attributes")
        {
            attributeName = component[4];

            // x = entityTypeSet(typeName, attributeName);
            restReply(fd, type, 400, "entityTypes", "not implemented");
        }
        else if (component[3] == "attributeDomains")
        {
            attributeDomainName = component[4];

            // x = entityTypeAttributeDomainSet(typeName, attributeDomainName);
            restReply(fd, type, 400, "entityTypeAttributeDomains", "not implemented");
        }        
        else
            restReply(fd, type, 400, "badPathComponent3", component[3]);
    }
    else if (component[1] == "contextSubscriptions")
    {
        std::string subscriptionId = component[2];

        // x = contextSubscriptionSet(subscriptionId);
        restReply(fd, type, 400, "contextSubscriptions", "not implemented");
    }
    else if (component[1] == "queryContext")
    {
        restReply(fd, type, 400, "queryContext", "not implemented");
    }
    else if (component[1] == "subscribeContext")
    {
        restReply(fd, type, 400, "subscribeContext", "not implemented");
    }
    else if (component[1] == "updateContextSubscription")
    {
        restReply(fd, type, 400, "updateContextSubscription", "not implemented");
    }
    else if (component[1] == "unsubscribeContext")
    {
        restReply(fd, type, 400, "unsubscribeContext", "not implemented");
    }
    else if (component[1] == "updateContext")
    {
        restReply(fd, type, 400, "updateContext", "not implemented");
    }
    else
    {
        restReply(fd, type, 400, "unknown", component[1].c_str());
        return -1;
    }

    return 0;
}



/* ****************************************************************************
*
* restServe - 
*/
int restServe(int fd)
{
    int nb;

    nb = read(fd, inBuf, sizeof(inBuf));
    if (nb == -1)
        LM_RE(-1, ("read: %s", strerror(errno)));
    else if (nb == 0)
        LM_RE(-1, ("read: connection closed"));
    else
    {
        char* lineV[100];
        int   lines;

        lines = lineSplit(inBuf, lineV);
        linesPresent(lines, lineV);
        if (strncmp(lineV[0], "GET ", 4) == 0)
            restTreat(GET, &lineV[0][4], fd);
        else if (strncmp(lineV[0], "POST ", 5) == 0)
            restTreat(POST, &lineV[0][5], fd);
        else if (strncmp(lineV[0], "PUT ", 4) == 0)
            restTreat(PUT, &lineV[0][5], fd);
        else if (strncmp(lineV[0], "DELETE ", 7) == 0)
            restTreat(DELETE, &lineV[0][7], fd);
        else
        {
            LM_E(("Bad operation: '%s'", lineV[0]));
        }
    }

    return 0;
}



/* ****************************************************************************
*
* serve - 
*/
void serve(int fd)
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
			
			if (cFd != -1)
				close(cFd);
			cFd = accept(fd, (struct sockaddr*) &sin, &len);
			if (cFd == -1)
				LM_X(1, ("accept: %s", strerror(errno)));
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

    paConfig("builtin prefix",                    (void*) "SS_NGSI_");
    paConfig("usage and exit on any warning",     (void*) true);
    paConfig("log to screen",                     (void*) true);

    paConfig("log file line format",              (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE](p.PID)(t.TID) FUNC: TEXT");
    paConfig("screen line format",                (void*) "TYPE@TIME  EXEC: TEXT");

    paParse(paArgs, argC, (char**) argV, 1, false);

    if (fg == false)
        daemonize();

    fd = serverInit(port);
    if (fd == -1)
        LM_X(1, ("error opening port %d for connections", port));

    LM_F(("Opened port %d for connections", port));
    serve(fd);

    return 0;
}

/* ****************************************************************************
*
* FILE                     ListenerEndpoint.h
*
* DESCRIPTION              Class for listener endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 27 2011
*
*/
#include <unistd.h>             // close
#include <fcntl.h>              // F_SETFD
#include <sys/socket.h>
#include <netdb.h>

#include "EndpointManager.h"    // EndpointManager
#include "UnhelloedEndpoint.h"  // UnhelloedEndpoint
#include "ListenerEndpoint.h"   // Own interface



namespace samson
{



/* ****************************************************************************
*
* ip2string - convert integer ip address to string
*/
static void ip2string(int ip, char* ipString, int ipStringLen)
{
	snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
			 ip & 0xFF,
			 (ip & 0xFF00) >> 8,
			 (ip & 0xFF0000) >> 16,
			 (ip & 0xFF000000) >> 24);
}



/* ****************************************************************************
*
* ListenerEndpoint - 
*/
ListenerEndpoint::ListenerEndpoint
(
	EndpointManager*  _epMgr,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Listener, 0, _host, _port, _rFd, _wFd)
{
	if (init() != Endpoint2::OK)
		LM_X(1, ("Error setting up listen socket for endpoint '%s'", name()));
}



/* ****************************************************************************
*
* ~ListenerEndpoint - 
*/
ListenerEndpoint::~ListenerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* init - 
*/
Endpoint2::Status ListenerEndpoint::init(void)
{
	int                 reuse = 1;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if (type != Listener)
		LM_RE(NotListener, ("Cannot prepare an Endpoint that is not a listener ..."));

	if (rFd != -1)
		LM_W(("This listener already seems to be prepared ... Continuing anuway"));

	if ((rFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RP(SocketError, ("socket"));

	fcntl(rFd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(rFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(rFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		::close(rFd);
		rFd = -1;
		LM_RP(BindError, ("bind to port %d: %s", port, strerror(errno)));
	}

	if (listen(rFd, 10) == -1)
	{
		::close(rFd);
		rFd = -1;
		LM_RP(ListenError, ("listen to port %d", port));
	}

	state = Endpoint2::Ready;
	return OK;
}



static void newWayCheck(struct sockaddr_in sin, int len)
{
	int   s;
	char  hostname[128];
	char  servicename[128];
	int   flags;

	// 1. Get name with domain
	flags = 0;
	memset(hostname,    0, sizeof(hostname));
	memset(servicename, 0, sizeof(servicename));
	s = getnameinfo((const struct sockaddr*) &sin, len, hostname, sizeof(hostname), servicename, sizeof(servicename), flags);
	LM_M(("Name: '%s', Service: '%s'", hostname, servicename));

	// 2. Get name without domain
	flags = NI_NOFQDN;
	memset(hostname,    0, sizeof(hostname));
	memset(servicename, 0, sizeof(servicename));
	s = getnameinfo((const struct sockaddr*) &sin, len, hostname, sizeof(hostname), servicename, sizeof(servicename), flags);
	LM_M(("Name: '%s', Service: '%s'", hostname, servicename));

	// 3. Numeric form of the hostname
	flags = NI_NUMERICHOST;
	memset(hostname,    0, sizeof(hostname));
	memset(servicename, 0, sizeof(servicename));
	s = getnameinfo((const struct sockaddr*) &sin, len, hostname, sizeof(hostname), servicename, sizeof(servicename), flags);
	LM_M(("Name: '%s', Service: '%s'", hostname, servicename));

}



/* ****************************************************************************
*
* accept - 
*/
UnhelloedEndpoint* ListenerEndpoint::accept(void)
{
	int                 fd;
	struct sockaddr_in  sin;
	char                hostName[64];
	unsigned int        len         = sizeof(sin);
	int                 hostNameLen = sizeof(hostName);
	UnhelloedEndpoint*  ep          = NULL;

	memset((char*) &sin, 0, len);

	LM_T(LmtAccept, ("Accepting incoming connection"));
	if ((fd = ::accept(rFd, (struct sockaddr*) &sin, &len)) == -1)
		LM_RP(NULL, ("accept"));

	newWayCheck(sin, len);

	ip2string(sin.sin_addr.s_addr, hostName, hostNameLen);

	Host* hostP = epMgr->hostMgr->lookup(hostName);
	if (hostP == NULL)
		hostP = epMgr->hostMgr->insert(NULL, hostName);

	LM_T(LmtAccept, ("Creating new Unhelloed Endpoint"));
	ep = new UnhelloedEndpoint(epMgr, hostP, 0, fd, fd);
	ep->state = Connected;
	epMgr->add(ep);

	return ep;
}



/* ****************************************************************************
*
* msgTreat - 
*/
Endpoint2::Status ListenerEndpoint::msgTreat2(void)
{
	UnhelloedEndpoint* ep;

	LM_T(LmtAccept, ("Accepting an incoming connection"));
	ep = accept();
	if (ep == NULL)
		LM_RE(AcceptError, ("Endpoint2::accept returned NULL"));

	LM_T(LmtHello, ("Sending hello to %s", ep->name()));
	ep->helloSend(Message::Msg);
	ep->state = Ready;
	return Endpoint2::OK;
}

}

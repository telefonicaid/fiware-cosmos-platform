/* ****************************************************************************
*
* FILE                      Endpoint.cpp
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string.h>             // strchr
#include <stdlib.h>             // atoi

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*, ...

#include "Endpoint.h"           // Own interface



namespace ss {


/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(std::string name, std::string ip, unsigned short port, int fd)
{
	this->name  = name;
	this->ip    = ip;
	this->port  = port;
	this->fd    = fd;
	this->state = (fd == -1)? Taken : Connected;
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(std::string ipAndPort)
{
	char* port = strchr((char*) ipAndPort.c_str(), ':');

	if (port == NULL)
	{
		this->name  = "local accepter";
		this->port  = atoi(ipAndPort.c_str());
		this->ip    = "127.0.0.1";
	}
	else
	{
		this->name  = ipAndPort;

		*port = 0;
		++port;
		this->ip    = ipAndPort;
		this->port  = atoi(port);
	}

	this->fd    = -1;
	this->state = Taken;
}



/* ****************************************************************************
*
* Constructor
*/
Endpoint::Endpoint(unsigned short port)
{
	this->name  = "no name";
	this->ip    = "localhost";
	this->port  = port;
	this->fd    = -1;
	this->state = Taken;
}



/* ****************************************************************************
*
* stateName - 
*/
char* Endpoint::stateName(void)
{
	switch (state)
	{
	case Free:            return (char*) "Free";
	case Taken:           return (char*) "Taken";
	case Connected:       return (char*) "Connected";
	case Listening:       return (char*) "Listening";
	case Disconnected:    return (char*) "Disconnected";
	}

	return (char*) "Unknown";
}



/* ****************************************************************************
*
* reset - 
*/
void Endpoint::reset(void)
{
	close(fd);
	fd = -1;
	state = Taken;
}

}

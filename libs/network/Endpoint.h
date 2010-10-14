#ifndef ENDPOINT_H
#define ENDPOINT_H

/* ****************************************************************************
*
* FILE                      Endpoint.h
*
* DESCRIPTION				Class for endpoints
*
*/
#include <string>	        // std::string...



namespace ss {


class Endpoint
{
public:
	typedef enum State
	{
		Free,
		Taken,
		Connected,
		Listening,
		Disconnected
	} State;

public:
	std::string     name;
	std::string     ip;
	int             fd;
	unsigned short  port;
	State           state;
	char*           stateName(void);
	void            reset();

public:
	Endpoint(unsigned short port);
	Endpoint(std::string ipAndPort);
	Endpoint(std::string name, std::string ip, unsigned short port, int fd);
	std::string str() { return name; }
};

}

#endif

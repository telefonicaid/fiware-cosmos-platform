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
		Disconnected,
		Reconnecting
	} State;

	typedef enum Type
	{
		Unknown,
		Temporal,
		Listener,
		Controller,
		Worker,
		Delilah
	} Type;

public:
	std::string     name;
	std::string     hostname;
	std::string     ip;
	int             fd;
	unsigned short  port;
	State           state;
	int             workers;
	Type            type;

	char*           stateName(void);
	void            reset();
	void            hostnameGet(void);

public:
	Endpoint(void) {};
	Endpoint(Type type, unsigned short port);
	Endpoint(Type type, std::string ipAndPort);
	Endpoint(Type type, std::string name, std::string ip, unsigned short port, int fd);
	std::string str() { return name; }
};

}

#endif

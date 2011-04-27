#ifndef ENDPOINT_MANAGER_H
#define ENDPOINT_MANAGER_H

/* ****************************************************************************
*
* FILE                     EndpointManager.h - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 06 2011
*
*/
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Process.h"            // ProcessVector
#include "Endpoint2.h"          // Endpoint2



namespace ss
{



/* ****************************************************************************
*
* EndpointManager - 
*/
class EndpointManager
{
private:
	Endpoint2**     endpoint;
	unsigned int    endpoints;
	unsigned int    workers;

	int               tmoSecs;
	int               tmoUSecs;

public:
	ProcessVector*  procVec;
	HostMgr*        hostMgr;

	Endpoint2*         me;
	Endpoint2*         controller;
	ListenerEndpoint*  listener;
	ListenerEndpoint*  webListener;

	EndpointManager(Endpoint2::Type _type, unsigned int _endpoints = -1, const char* controllerIp = NULL);
	~EndpointManager();

	void            setupAwait(void);
	Endpoint2*      add(Endpoint2* ep);
	Endpoint2*      add(Endpoint2::Type type, int id, const char* name, const char* alias, Host* host, unsigned short port, int rFd, int wFd);
	void            remove(Endpoint2* ep);
	Endpoint2*      get(unsigned int index);
	Endpoint2*      get(unsigned int index, int* rFdP);
	Endpoint2*      lookup(Endpoint2::Type type, const char* ip);
	Endpoint2*      lookup(const char* alias);
	void            list(const char* why, bool forced = false);
	
	void            tmoSet(int secs, int usecs);  // Set timeout for select loop
	void            run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...
};

}

#endif

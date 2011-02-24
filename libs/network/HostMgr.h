#ifndef HOST_MGR_H
#define HOST_MGR_H

/* ****************************************************************************
*
* FILE                     HostMgr.h
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 10 2011
*
*/
#include "Host.h"               // Host



namespace ss
{



/* ****************************************************************************
*
* HostMgr
*/
class HostMgr
{
public:
	HostMgr(unsigned int size);
	~HostMgr();

	int     hosts();
	Host*   insert(Host*);
	Host*   insert(const char* name, const char* ip);
	Host*   lookup(const char* ip);
	bool    match(Host* host, const char* ip);
	void    aliasAdd(Host* host, const char* alias);
	void    ipSet(Host* host, const char* alias);
	void    list(const char* why);

private:
	void          localIps(void);
	void          ipsGet(Host* hostP);

	Host**        hostV;
	unsigned int  size;
};

}

#endif

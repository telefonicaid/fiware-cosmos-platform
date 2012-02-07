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

#include "au/vector.h"

#include "Host.h"               // Host



namespace samson
{



/* ****************************************************************************
*
* HostMgr
*/
class HostMgr
{
public:
	HostMgr();
	~HostMgr();

	int     hosts();
	Host*   insert(Host*);
	Host*   insert(const char* name, const char* ip);
	bool    remove(const char* name);
	Host*   lookup(const char* ip);
	void    aliasAdd(Host* host, const char* alias);
	void    ipSet(Host* host, const char* alias);
	void    list(const char* why, bool forced = false);
	Host*   localhostP;

private:
	void          localIps(void);
	void          ipsGet(Host* hostP);

    au::vector<Host> hostV;
};

}

#endif

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



/* ****************************************************************************
*
* Host - 
*/
typedef struct Host
{
	char* hostName;
	char* ip;
	char* alias[10];
} Host;



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
	void    insert(char* ip);
	Host*   lookup(char* ip);
	void    aliasAdd(Host* host, char* alias);
	void    ipSet(Host* host, char* alias);

private:
	Host**        hostV;
	unsigned int  size;
};

#endif

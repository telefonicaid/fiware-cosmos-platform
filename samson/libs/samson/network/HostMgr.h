/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
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

#include "au/containers/vector.h"

#include "Host.h"               // Host



namespace samson
{



/* ****************************************************************************
*
* HostMgr
*/
class HostMgr
{
    
    au::vector<Host> hosts;
/*
    void          localIps(void);
*/
    

public:
    
	HostMgr();
	~HostMgr();

    void add( std::string host_name );
	void add(Host*);
    void add_localhost();

    Host* findHost( std::string host_name );
    std::string str();

    std::string getAlternative( std::string host_name );
    
    /*
	int     hosts();
	Host*   insert(const char* name, const char* ip);
	bool    remove(const char* name);
	Host*   lookup(const char* ip);
	void    ipSet(Host* host, const char* alias);
	void    list(const char* why, bool forced = false);
*/
     

};

}

#endif

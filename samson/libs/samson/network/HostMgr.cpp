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
/* ****************************************************************************
 *
 * FILE                     HostMgr.cpp
 *
 * AUTHOR                   Ken Zangelin
 *
 * CREATION DATE            Feb 10 2011
 *
 */
#include <stdio.h>              // popen
#include <unistd.h>             // gethostname
#include <arpa/inet.h>          // sockaddr_in, inet_ntop
#include <ifaddrs.h>            // getifaddrs
#include <net/if.h>             // IFF_UP
#include <netdb.h>              // 
#include <string.h>             // strstr

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "au/network/misc.h"

#include "Host.h"               // Host
#include "HostMgr.h"            // Own interface
#include <stdlib.h>             // free



namespace samson
{
    
    
    
    /* ****************************************************************************
     *
     * HostMgr
     */
    HostMgr::HostMgr()
    {
        LM_T(LmtHost, ("Creating Host Manager"));
        
        //localIps();
    }
    
    
    
    /* ****************************************************************************
     *
     * ~HostMgr
     */
    HostMgr::~HostMgr()
    {
        hosts.clearVector();
    }
    
    
    
    /* ****************************************************************************
     *
     * ipsGet - get all IPs for a machine
     */
    
    void HostMgr::add_localhost()
    {
        struct ifaddrs*      addrs;
        struct ifaddrs*      iap;
        struct sockaddr_in*  sa;
        char                 buf[64];
        
        Host *host = new Host("localhost");
        
        getifaddrs(&addrs);
        for (iap = addrs; iap != NULL; iap = iap->ifa_next)
        {
            if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) && iap->ifa_addr->sa_family == AF_INET)
            {
                sa = (struct sockaddr_in*)(iap->ifa_addr);
                inet_ntop(iap->ifa_addr->sa_family, (void*) &(sa->sin_addr), buf, sizeof(buf));
                
                if (strcmp(buf, "127.0.0.1") != 0)
                    host->addAlias(buf);
            }
        }
        
        add( host );
        
        freeifaddrs(addrs);
    }

    std::string HostMgr::getAlternative( std::string host_name )
    {
        Host* host = findHost(host_name);
        if( !host )
            return host_name;
        
        return host->getAlternative( host_name );
    }
    
    
    // Add 
    
	void HostMgr::add( Host* host )
    {
        if( host->getNumAliases() == 0 )
        {
            delete host;
            return;
        }
        
        std::vector<std::string> aliases = host->getAllAliases();
        for( size_t i = 0 ; i < aliases.size() ; i++)
            if( findHost( aliases[i] ) != NULL )
            {
                LM_W(("Not adding host %s since it was already instroduced" , aliases[i].c_str() ));
                
                delete host;
                return;
            }
        
        hosts.push_back( host );
    }
    
    
    Host* HostMgr::findHost( std::string host_name )
    {
        for ( size_t i = 0 ; i < hosts.size() ; i++ )
            if( hosts[i]->match( host_name ) )
               return hosts[i];
               
        return NULL;
    }
    
    /* ****************************************************************************
     *
     * localIps - 
     */
    /*
    void HostMgr::localIps(void)
    {
        char   hostName[128];
        char   domain[128];
        char   domainedName[128];
        
        while (1)
        {
            if (gethostname(hostName, sizeof(hostName)) == -1)
                LM_X(1, ("gethostname: %s", strerror(errno)));
            
            if (strcmp(hostName, "localhost") == 0)
            {
                LM_W(("Host name 'localhost' - awaiting an allowed host name ..."));
                sleep(1);
                continue;
            }
            
            break;
        }
        
        LM_T(LmtHost, ("hostName: '%s'", hostName));
        localhostP = insert(hostName, "127.0.0.1"); // , "127.0.0.1" ...
        
        memset(domainedName, 0, sizeof(domainedName));
        if (getdomainname(domain, sizeof(domain)) == -1)
            LM_X(1, ("getdomainname: %s", strerror(errno)));
        
        LM_TODO(("Would gethostname ever return the 'domained' name ?"));
        
        if (domainedName[0] != 0)
        {
            snprintf(domainedName, sizeof(domainedName), "%s.%s", hostName, domain);
            aliasAdd(localhostP, domainedName);
        }
        
        aliasAdd(localhostP, "localhost");
        
        ipsGet(localhostP);
    }
    
    */
    
    /* ****************************************************************************
     *
     * HostMgr::hosts - 
     */
    /*
    int HostMgr::hosts(void)
    {
        return hostV.size();
    }
     */
    
    
    
    
/*    
    static void newCheck(const char* name, const char* ip)
    {
        int               s;
        char*             node = (char*) name;
        struct addrinfo*  result;
        struct addrinfo*  res;
        
        if (node == NULL)
            node = (char*) ip;
        
        s = getaddrinfo(node, NULL, NULL, &result);
        if (s == 0)
        {
            for (res = result; res != NULL; res = res->ai_next)
            {   
                char hostname[NI_MAXHOST] = "";
                
                s = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
                if (s != 0)
                {
                    LM_E(("error in getnameinfo: %s", gai_strerror(s)));
                    continue;
                }
            }
            
            freeaddrinfo(result);
        }
    }
    
    */
    
    /* ****************************************************************************
     *
     * HostMgr::insert - 
     */
    void HostMgr::add( std::string host_name )
    {
        char   ipX[64];

        // Check if we already have this host
        if( findHost(host_name) != NULL )
            return;
        

        Host * host = new Host( host_name );
        
        // Get host information
        struct hostent* heP;
        heP = gethostbyname( host_name.c_str() );
        
        if (heP == NULL)
            LM_W(("gethostbyname(%s) error", host_name.c_str() ));
        else
        {
            int ix = 0;
            
            au::ip2string(*((int*) heP->h_addr_list[ix]), ipX, sizeof(ipX));
            host->addAlias(ipX);
            host->addAlias(heP->h_name);

            /*
            while (heP->h_aliases[ix] != NULL)
            {
                LM_TODO(("alias %d: '%s' - should also be added?", ix, heP->h_aliases[ix]));
                ++ix;
            }
             */

            /*
            for (ix = 1; ix < heP->h_length / 4; ix++)
            {
                if (heP->h_addr_list[ix] != NULL)
                {
                    char ipY[64];
                    ip2string(*((int*) heP->h_addr_list[ix]), ipX, sizeof(ipX));
                    LM_TODO(("addr %d: '%s' should also be added?", ix, ipY));
                }
            }
             */
        }
        
        /*
        if (name != NULL)
        {
            if ((dotP = (char*) strstr(name, ".")) != NULL)
            {
                if (onlyDigitsAndDots(name) == false)
                {
                    LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", name));
                    *dotP = 0;
                    LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", name));
                    aliasAdd(hostP, name);
                }
            }
        }
        
        if (ip != NULL)
        {
            if ((dotP = (char*) strstr(ip, ".")) != NULL)
            {
                if (onlyDigitsAndDots(ip) == false)
                {
                    LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", ip));
                    *dotP = 0;
                    LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", ip));
                    aliasAdd(hostP, ip);
                }
            }
        }
         */
        
        return add( host );
    }
    
    
    
    
    

    /* ****************************************************************************
     *
     * list - list the hosts in the list
     */
    std::string HostMgr::str()
    {
        std::ostringstream output;       
        
        for ( size_t i = 0 ; i < hosts.size() ; i++ )
            output << hosts[i]->str() << "\n";
        return output.str();
    }
    
    
}

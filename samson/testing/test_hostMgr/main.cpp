#include <unistd.h>             // gethostname

#include "parseArgs.h"          // parseArgs
#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace levels

#include "Host.h"               // Host
#include "HostMgr.h"            // Network

#include <stdlib.h>             // free()


/* ****************************************************************************
*
* External declarations
*/
extern "C" void exit(int);



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	PA_END_OF_ARGS
};


#define HOSTS 10
/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	samson::HostMgr*  hostMgr = new samson::HostMgr(HOSTS);
	Host*         hostP;
	Host*         localhostP;
	char          hostname[128];
	char          name[32];
	char          ip[23];
	int           ix;

	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);


	//
	// Creating the list
	//
	hostMgr = new samson::HostMgr(HOSTS);
	if (hostMgr == NULL)
		LM_X(1, ("Error creating the Host Manager"));



	//
	// Looking up 'localhost'
	//
	hostP = hostMgr->lookup("localhost");
	if (hostP == NULL)
		LM_X(2, ("Cannot find 'localhost'"));



	//
	// Inserting 'localhost' again
	//
	gethostname(hostname, sizeof(hostname));
	localhostP = hostMgr->insert(hostname, NULL);
	if (localhostP != hostP)
		LM_X(3, ("Inserted localhost '%s' again ...", hostname));



	//
	// Looking up 'localhost' again
	//
	hostP = hostMgr->lookup("localhost");
	if (hostP == NULL)
		LM_X(4, ("Cannot find 'localhost'"));

	LM_M(("localhost:  name:'%s', ip:'%s'", hostP->name, hostP->ip));



	//
	// Filling the list
	//
	for (ix = 1; ix < HOSTS; ix++)
	{
		snprintf(name, sizeof(name), "nada%03d", ix);
		snprintf(ip,   sizeof(ip),   "5.5.5.%03d", ix);
		
		hostP = hostMgr->insert(name, ip);
		if (hostP == NULL)
			LM_X(5, ("Error inserting non-existing host '%s' with ip '%s'", name, ip));
	}



	//
	// Trying to insert one more host - this should fail!
	//
	snprintf(name, sizeof(name), "nada%03d", ix);
	snprintf(ip,   sizeof(ip),   "5.5.5.%03d", ix);
		
	hostP = hostMgr->insert(name, ip);
	if (hostP != NULL)
		LM_X(6, ("OK inserting one too many hosts - the host vector should be full!"));



	//
	// Removing host nada001
	//
	int hostsBefore = hostMgr->hosts();
	if (hostMgr->remove("nada001") == false)
		LM_X(7, ("Error removing host 'nada001'"));

	int hostsAfter = hostMgr->hosts();
	if (hostsAfter != hostsBefore - 1)
		LM_X(8, ("Bad host count after remove. Got %d, should be %d", hostsBefore - 1, hostsAfter));

	
	//
	// Trying to insert one more host again - now this should NOT fail!
	//
	snprintf(name, sizeof(name), "nada%03d", ix);
	snprintf(ip,   sizeof(ip),   "5.5.5.%03d", ix);
		
	hostP = hostMgr->insert(name, ip);
	if (hostP == NULL)
		LM_X(9, ("Error inserting non-existing host '%s' with ip '%s'", name, ip));



	//
	// Test OK
	//
	LM_M(("Test OK"));
	return 0;
}

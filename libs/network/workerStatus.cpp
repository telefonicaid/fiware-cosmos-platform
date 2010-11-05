/* ****************************************************************************
*
* FILE                      workerStatus.cpp
*
* DESCRIPTION               extraction of machine information
*
*/
#include <stdio.h>              // FILE, fopen, ...
#include <errno.h>              // errno
#include <string.h>             // strerror
#include <stdlib.h>             // atoi
#include <time.h>               // time

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*

#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "workerStatus.h"       // Own interface



namespace ss {

class Network;

namespace Message
{



/* ****************************************************************************
*
* Definitions
*/
#define CPUINFO_PATH     "/proc/cpuinfo"
#define STAT_PATH        "/proc/stat"
#define NETDEV_PATH      "/proc/net/dev"



/* ****************************************************************************
*
* CpuTimes - 
*/
typedef struct CpuTimes
{
    long  user;
    long  nice;
    long  system;
    long  idle;
    long  iowait;
    long  irq;
    long  softIrq;
    long  steal;
    long  guest;
} CpuTimes;



/* ****************************************************************************
*
* cLoad - measure the load of core 'coreNo'
*/
static int cLoad(int coreNo)
{
	FILE*            fP;
	char             line[160];
	static CpuTimes  oldticks[MAX_CORES + 1] = { { -1 } };
	CpuTimes         ticks;
	int              load;

	if (coreNo > MAX_CORES)
		LM_X(1, ("request for load on core %d - max core number is %d. Please redefine MAX_CORES and recompile", coreNo, MAX_CORES));

	fP = fopen(STAT_PATH, "r");
	if (fP == NULL)
		LM_RE(-1, ("fopen(%s): %s", STAT_PATH, strerror(errno)));
	
	char coreName[7];

	if (coreNo == MAX_CORES) /* Grand total */
		sprintf(coreName, "cpu ");
	else
		sprintf(coreName, "cpu%d ", coreNo);

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char coreName2[16];

		if (strncmp(line, coreName, strlen(coreName)) != 0)
			continue;

		sscanf(line, "%s %ld %ld %ld %ld %ld %ld %ld %ld",
			   coreName2,
			   &ticks.user,
			   &ticks.nice,
			   &ticks.system,
			   &ticks.idle,
			   &ticks.iowait,
			   &ticks.irq,
			   &ticks.softIrq,
			   &ticks.steal);
		
		/* Do I have an old measurement?  - if not, I cannot measure the CPU load ... */
		if (oldticks[coreNo].user == -1)
		{
			fclose(fP);
			return 0;
		}

		CpuTimes* oldP = &oldticks[coreNo];

		long long total     = ticks.user + ticks.nice + ticks.system + ticks.idle + ticks.iowait + ticks.irq + ticks.softIrq + ticks.steal;
		long long oldTotal  = oldP->user + oldP->nice + oldP->system + oldP->idle + oldP->iowait + oldP->irq + oldP->softIrq + oldP->steal;
		long long totalDiff = total - oldTotal;
		
		if (totalDiff != 0)
		{
			long long idleDiff       = ticks.idle - oldticks[coreNo].idle;
			long long idlePercentage = (100 * idleDiff) / totalDiff;

			load = 100 - idlePercentage;
		}
		else
			load = 0;

		// Remember what was just measured for the next time
		oldticks[coreNo] = ticks;

		fclose(fP);
		return load;
	}

	LM_W(("core %d not found ...", coreNo));
	fclose(fP);
	return 0;
}



/* ****************************************************************************
*
* cores - 
*/
static int cores(void)
{
	FILE* fP;
	char  line[160];
	int   coreNo = -2;

	fP = fopen(CPUINFO_PATH, "r");
	if (fP == NULL)
		LM_RE(-1, ("fopen(%s): %s", CPUINFO_PATH, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char* colon;

		if (strncmp(line, "processor", 9) != 0)
			continue;

		colon = strchr(line, ':');
		if (colon == NULL)
			continue;
		++colon;
		coreNo = atoi(colon);
	}

	fclose(fP);
	return coreNo + 1;
}



/* ****************************************************************************
*
* coreInfo - 
*/
static void coreInfo(int coreNo, CoreInfo* ciP)
{
	ciP->load = cLoad(coreNo);

	FILE* fP;
	char  line[160];
	int   cNo = -1;

	fP = fopen(CPUINFO_PATH, "r");
	if (fP == NULL)
		LM_RVE(("fopen(%s): %s", CPUINFO_PATH, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char* colon;

		if (strncmp(line, "processor", 9) == 0)
		{
			colon = strchr(line, ':');
			if (colon == NULL)
				continue;
			++colon;
			cNo = atoi(colon);

			if (cNo > coreNo)
			{
				fclose(fP);
				return;
			}
		}
		else if (cNo == coreNo)
		{
			if (strncmp(line, "cpu MHz", 7) == 0)
			{
				colon = strchr(line, ':');
				if (colon == NULL)
					continue;
				++colon;
				ciP->mhz = atoi(colon);
			}
			else if (strncmp(line, "bogomips", 8) == 0)
			{
				colon = strchr(line, ':');
				if (colon == NULL)
					continue;
				++colon;
				ciP->bogomips = atoi(colon);
			}
			else if (strncmp(line, "cache size", 10) == 0)
			{
				colon = strchr(line, ':');
				if (colon == NULL)
					continue;
				++colon;
				ciP->cacheSize = atoi(colon);
			}
		}
	}

	fclose(fP);
}



/* ****************************************************************************
*
* netifs - 
*/
static int netifs(void)
{
	FILE* fP;
	char  line[160];
	int   ifs = 0;

	fP = fopen(NETDEV_PATH, "r");
	if (fP == NULL)
		LM_RE(-1, ("fopen(%s): %s", CPUINFO_PATH, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		char* colon;

		colon = strchr(line, ':');
		if (colon == NULL)
			continue;
		++ifs;
	}

	fclose(fP);
	return ifs;
}



/* ****************************************************************************
*
* cpuInfo - 
*/
static void cpuInfo(CpuInfo* ciP)
{
	int cIx;

	ciP->cores = cores();
	ciP->load  = cLoad(MAX_CORES);

	for (cIx = 0; cIx < ciP->cores; cIx++)
	{
		memset(&ciP->coreInfo[cIx], 0, sizeof(ciP->coreInfo[cIx]));
		coreInfo(cIx, &ciP->coreInfo[cIx]);
	}
}



/* ****************************************************************************
*
* netifInfo - 
*/
static void netifInfo(int ifIndex, NetIf* nifP, time_t now, time_t lastTime)
{
	FILE*          fP;
	char           line[160];
	int            lineNo            = 0;
	static NetIf   netif[MAX_NETIFS] = { { { 0 }, 0 } };
	unsigned long  intervalInSecs    = now - lastTime;

	fP = fopen(NETDEV_PATH, "r");
	if (fP == NULL)
		LM_RVE(("fopen(%s): %s", NETDEV_PATH, strerror(errno)));

	while (fgets(line, sizeof(line), fP) != NULL)
	{
		++lineNo;

		if (lineNo - 3 != ifIndex)
		{
			continue;
		}

		char* colon;

		colon = strchr(line, ':');
		if (colon == NULL)
		{
			LM_W(("no colon in '%s'", line));
			continue;
		}
		*colon = 0;
		++colon;

		strcpy(nifP->name, line);
		sscanf(colon, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
			   &nifP->rcvBytes, &nifP->rcvPackets, &nifP->rcvErrors,     &nifP->rcvDrop,
			   &nifP->rcvFifo,  &nifP->rcvFrame,   &nifP->rcvCompressed, &nifP->rcvMulticast,
			   &nifP->sndBytes, &nifP->sndPackets, &nifP->sndErrs,       &nifP->sndDrop,
			   &nifP->sndFifo,  &nifP->sndColls,   &nifP->sndCarrier,    &nifP->sndCompressed);

		if ((lastTime == 0) || (intervalInSecs == 0))
		{
			if (intervalInSecs == 0)
				LM_W(("Zero interval is not very interesting ..."));

			nifP->rcvSpeed  = 0;
			nifP->sndSpeed  = 0;
		}
		else
		{
			if (netif[ifIndex].rcvBytes != 0)
				nifP->rcvSpeed = (nifP->rcvBytes - netif[ifIndex].rcvBytes) / intervalInSecs;
			if (netif[ifIndex].sndBytes != 0)
				nifP->sndSpeed = (nifP->sndBytes - netif[ifIndex].sndBytes) / intervalInSecs;
		}

		netif[ifIndex] = *nifP;
		fclose(fP);

		return;
	}

	fclose(fP);
}



/* ****************************************************************************
*
* netInfo - 
*/
static void netInfo(NetIfInfo* niP)
{
	int            nIx;
	time_t         now  = time(NULL);
	static time_t  then = 0;

	niP->ifaces = netifs();

	for (nIx = 0; nIx < niP->ifaces; nIx++)
	{
		memset(&niP->iface[nIx], 0, sizeof(niP->iface[nIx]));
		strcpy(niP->iface[nIx].name, "nada");
		netifInfo(nIx, &niP->iface[nIx], now, then);
	}

	then = now;
}


#if 0
/* ****************************************************************************
*
* coreWorkerInfo - 
*/
static void coreWorkerInfo(CoreWorkerInfo* cwiP, NetworkInterface* networkP)
{
	int        coreWorkers = 0;
	int        ix;
	Endpoint*  ep;
	int        Workers = networkP->getNumWorkers();

	for (ix = 3 + Workers; ix < (int) (sizeof(networkP->endpoint) / sizeof(networkP->endpoint[0])); ix++)
	{
		ep = networkP->endpoint[ix];

		if (ep == NULL)
			continue;

		if (ep->type == Endpoint::CoreWorker)
		{
			cwiP->worker[coreWorkers].coreNo     = ep->coreNo;
			cwiP->worker[coreWorkers].state      = ep->coreWorkerState;
			cwiP->worker[coreWorkers].jobsDone   = ep->jobsDone;
			cwiP->worker[coreWorkers].restarts   = ep->restarts;

			strncpy(cwiP->worker[coreWorkers].name, ep->name.c_str(), sizeof(cwiP->worker[coreWorkers].name));

			if (ep->coreWorkerState == Busy)
				cwiP->worker[coreWorkers].uptime = time(NULL) - ep->startTime;
			else
				cwiP->worker[coreWorkers].uptime = 0;

			++coreWorkers;
		}
	}

	cwiP->workers = coreWorkers;
}
#endif



/* ****************************************************************************
*
* workerStatus - 
*/
void workerStatus(WorkerStatusData* wsP)
{
	cpuInfo(&wsP->cpuInfo);
	netInfo(&wsP->netInfo);
	// coreWorkerInfo(&wsP->coreWorkerInfo, networkP);
}

/* ****************************************************************************
*
* coreWorkerState - 
*/
const char* coreWorkerState(CoreWorkerState state)
{
	switch (state)
	{
	case NotBusy:           return "Not Busy";
	case Busy:              return "Busy";
	}

	return "Unknown";
}

}
}

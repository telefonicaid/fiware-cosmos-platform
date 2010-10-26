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

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*

// #include "samson.pb.h"          // WorkerStatus
#include "workerStatus.h"       // Own interface

namespace ss {


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
static void coreInfo(int coreNo, ss::network::CoreInfo* ciP)
{
   ciP->set_load(cLoad(coreNo));

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
				ciP->set_mhz(atoi(colon));
			}
			else if (strncmp(line, "bogomips", 8) == 0)
			{
				colon = strchr(line, ':');
				if (colon == NULL)
					continue;
				++colon;
				ciP->set_bogomips(atoi(colon));
			}
			else if (strncmp(line, "cache size", 10) == 0)
			{
				colon = strchr(line, ':');
				if (colon == NULL)
					continue;
				++colon;
				ciP->set_cachesize(atoi(colon));
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
static void cpuInfo(ss::network::CpuInfo* ciP)
{
	int                    cIx;
	ss::network::CoreInfo  info;

	ciP->set_cores(cores());
	ciP->set_load(cLoad(MAX_CORES));

	for (cIx = 0; cIx < (int) ciP->cores(); cIx++)
	{
		info = ciP->coreinfo(cIx);
		info.set_mhz(0);
		info.set_bogomips(0);
		info.set_cachesize(0);
		info.set_load(0);


		coreInfo(cIx, &info);
	}
}



typedef struct NetInterface
{
	long rcvBytes;
	long rcvPackets;
	long rcvErrors;
	long rcvDrop;
	long rcvFifo;
	long rcvFrame;
	long rcvCompressed;
	long rcvMulticast;

	long sndBytes;
	long sndPackets;
	long sndErrs;
	long sndDrop;
	long sndFifo;
	long sndColls;
	long sndCarrier;
	long sndCompressed;
} NetInterface;



/* ****************************************************************************
*
* netifInfo - 
*/
static void netifInfo(int ifIndex, ss::network::NetIf* nifP, time_t now, time_t lastTime)
{
	FILE*                 fP;
	char                  line[160];
	static NetInterface   netif[MAX_NETIFS] = { { 0 } };
	int                   lineNo            = 0;
	unsigned long         intervalInSecs    = now - lastTime;

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

		nifP->set_name(line);
		long i[16];

		sscanf(colon, "%ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
			   &i[0],
			   &i[1],
			   &i[2],
			   &i[3],
			   &i[4],
			   &i[5],
			   &i[6],
			   &i[7],
			   &i[8],
			   &i[9],
			   &i[10],
			   &i[11],
			   &i[12],
			   &i[13],
			   &i[14],
			   &i[15]
			);

		nifP->set_rcvbytes(i[0]);
		nifP->set_rcvpackets(i[1]);
		nifP->set_rcverrors(i[2]);
		nifP->set_rcvdrop(i[3]);
		nifP->set_rcvfifo(i[4]);
		nifP->set_rcvframe(i[5]);
		nifP->set_rcvcompressed(i[6]);
		nifP->set_rcvmulticast(i[7]);
		nifP->set_sndbytes(i[8]);
		nifP->set_sndpackets(i[9]);
		nifP->set_snderrs(i[10]);
		nifP->set_snddrop(i[11]);
		nifP->set_sndfifo(i[12]);
		nifP->set_sndcolls(i[13]);
		nifP->set_sndcarrier(i[14]);
		nifP->set_sndcompressed(i[15]);

		if (lastTime == 0)
		{
			nifP->set_rcvspeed(0);
			nifP->set_sndspeed(0);
		}
		else
		{
			if (netif[ifIndex].rcvBytes != 0)
				nifP->set_rcvspeed((nifP->rcvbytes() - netif[ifIndex].rcvBytes) / intervalInSecs);
			if (netif[ifIndex].sndBytes != 0)
				nifP->set_sndspeed((nifP->sndbytes() - netif[ifIndex].sndBytes) / intervalInSecs);
		}

		netif[ifIndex].rcvBytes       = nifP->rcvbytes();
		netif[ifIndex].rcvPackets     = nifP->rcvpackets();
		netif[ifIndex].rcvErrors      = nifP->rcverrors();
		netif[ifIndex].rcvDrop        = nifP->rcvdrop();
		netif[ifIndex].rcvFifo        = nifP->rcvfifo();
		netif[ifIndex].rcvFrame       = nifP->rcvframe();
		netif[ifIndex].rcvCompressed  = nifP->rcvcompressed();
		netif[ifIndex].rcvMulticast   = nifP->rcvmulticast();
		netif[ifIndex].sndBytes       = nifP->sndbytes();
		netif[ifIndex].sndPackets     = nifP->sndpackets();
		netif[ifIndex].sndErrs        = nifP->snderrs();
		netif[ifIndex].sndDrop        = nifP->snddrop();
		netif[ifIndex].sndFifo        = nifP->sndfifo();
		netif[ifIndex].sndColls       = nifP->sndcolls();
		netif[ifIndex].sndCarrier     = nifP->sndcarrier();
		netif[ifIndex].sndCompressed  = nifP->sndcompressed();

		fclose(fP);

		return;
	}

	fclose(fP);
}



/* ****************************************************************************
*
* netInfo - 
*/
static void netInfo(ss::network::NetIfInfo* niP)
{
	int                 nIx;
	time_t              now  = time(NULL);
	static time_t       then = 0;
	ss::network::NetIf  netif;

	niP->set_ifaces(netifs());

	for (nIx = 0; nIx < (int) niP->ifaces(); nIx++)
	{
		netif = niP->iface(nIx);
		netif.Clear();
		// memset(&niP->iface[nIx], 0, sizeof(niP->iface[nIx]));
		netif.set_name("nada");
		netifInfo(nIx, &netif, now, then);
	}

	then = now;
}



/* ****************************************************************************
*
* workerStatus - 
*/
void workerStatus(ss::network::WorkerStatus* wsP)
{
	cpuInfo((ss::network::CpuInfo*) &wsP->cpuinfo());
	netInfo((ss::network::NetIfInfo*) &wsP->netinfo());
}

}

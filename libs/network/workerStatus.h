#ifndef WORKER_STATUS_H
#define WORKER_STATUS_H

/* ****************************************************************************
*
* FILE                      workerStatus.h
*
* DESCRIPTION				extraction of machine information
*
*/


namespace ss {



/* ****************************************************************************
*
* Definitions
*/
#define MAX_CORES        32
#define MAX_NETIFS       16



/* ****************************************************************************
*
* CoreInfo - structure describing a CPU core
*/
typedef struct CoreInfo
{
	int mhz;
	int bogomips;
	int cacheSize;
	int load;
} CoreInfo;



/* ****************************************************************************
*
* CpuInfo
*/
typedef struct CpuInfo
{
	int       load;
	int       cores;
	CoreInfo  coreInfo[MAX_CORES];
} CpuInfo;



/* ****************************************************************************
*
* NetIf - 
*/
typedef struct NetIf
{
	char  name[32];

	long  rcvSpeed;
	long  rcvBytes;
	long  rcvPackets;
	long  rcvErrors;
	long  rcvDrop;
	long  rcvFifo;
	long  rcvFrame;
	long  rcvCompressed;
	long  rcvMulticast;

	long  sndSpeed;
	long  sndBytes;
    long  sndPackets;
	long  sndErrs;
	long  sndDrop;
	long  sndFifo;
	long  sndColls;
	long  sndCarrier;
	long  sndCompressed;
} NetIf;



/* ****************************************************************************
*
* NetIfInfo - 
*/
typedef struct NetIfInfo
{
	int   ifaces;
	NetIf iface[MAX_NETIFS];
} NetIfInfo;



/* ****************************************************************************
*
* WorkerStatus
*/
typedef struct WorkerStatus
{
	CpuInfo    cpuInfo;
	NetIfInfo  netInfo;
} WorkerStatus;



/* ****************************************************************************
*
* workerStatus
*/
extern void workerStatus(WorkerStatus* wsP);

}

#endif

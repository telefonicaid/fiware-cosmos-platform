/* ****************************************************************************
*
* FILE                     main_samsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <stdio.h>              // printf, ...
#include <sys/stat.h>           // struct stat
#include <unistd.h>             // stat()
#include <fcntl.h>              // open, O_RDWR, O_CREAT, O_TRUNC, ...

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "samsonDirectories.h"  // SAMSON_IMAGES
#include "ports.h"              // WORKER_PORT
#include "platformProcesses.h"  // ss::platformProcessesGet, ss::platformProcessesSave
#include "Process.h"            // Process
#include <stdlib.h>             // free()



/* ****************************************************************************
*
* Option variables
*/
int    workers;
char*  ip[100];



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-ips",      ip,         "IP_LIST",   PaSList,   PaOpt,  PaND,   PaNL,  PaNL,  "listen port"         },
	{ "-workers",  &workers,   "WORKERS",   PaInt,     PaOpt,     0,     0,   100,   "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int         logFd                  = -1;
const char* EtcDirPath             = SAMSON_ETC;
const char* PlatformProcessesPath  = SAMSON_PLATFORM_PROCESSES;
const char* ppFile                 = PlatformProcessesPath;



/* ****************************************************************************
*
* accessCheck - 
*
* CASES
* 0. directory doesn't exist
* 1. No write permissions on directory
* 2. file doesn't exist and we have write permissions on directory
* 3. file exists 
*/
static int accessCheck(void)
{
	struct stat statBuf;

	if (stat(EtcDirPath, &statBuf) == -1)
	{
		if (errno == ENOENT)
		{
			printf("Samson Platform Setup Error.\n"
				   "The Samson Platform Directory '%s' doesn't exist - please create it and try again.\n", EtcDirPath);
			return 3;
		}
	}

	if (access(EtcDirPath, W_OK) == -1)
	{
		printf("Samson Platform Setup Error.\n"
			   "No permissions to create files under the samson platform directory '%s'.\n"
			   "Please fix the problem and try again.\n",
			   EtcDirPath);

		return 4;
	}

	if (access(PlatformProcessesPath, W_OK) == -1)
	{
		if (errno != ENOENT)
		{
			printf("Samson Platform Setup Error.\n"
				   "Cannot access the platform process file '%s' with write permissions: %s\n"
				   "Please fix the problem and try again.\n",
				   PlatformProcessesPath, strerror(errno));

			return 5;
		}
	}

	if (access(PlatformProcessesPath, R_OK) == 0)
	{
		char answer[32];

		printf("Samson Platform Setup Error.\n"
			   "The platform process file '%s' already exists.\n"
			   "Do you wish to overwrite it (Y/N)?> ", PlatformProcessesPath);
		fflush(stdout);
		scanf("%s", answer);
		if (((answer[0] == 'N') || (answer[0] == 'n')) && (answer[1] == 0))
			return 6;
		else if (((answer[0] == 'Y') || (answer[0] == 'y')) && (answer[1] == 0))
		{
			if (unlink(PlatformProcessesPath) == -1)
			{
				printf("Samson Platform Setup Error.\n"
					   "Unable to remove the Samson Platform Setup file '%s': %s\n",
					   PlatformProcessesPath, strerror(errno));

				return 7;
			}
		}
		else
		{
			printf("Samson Platform Setup Error.\n"
				   "Incorrect answer (%s).\n", answer);
			return 8;
		}
	}

	return 0;
}



/* ****************************************************************************
*
* platformFileCreate - 
*/
static int platformFileCreate(int workers, char* ip[])
{
	int                 s;
	int                 size;
	ss::ProcessVector*  pv;



	//
	// Checking access to the platform file
	//
	if ((s = accessCheck()) != 0)
		return s;



	//
	// Initializing variables for the worker vector
	//
	size = sizeof(ss::ProcessVector) + workers * sizeof(ss::Process);
	pv   = (ss::ProcessVector*) malloc(size);

	memset(pv, 0, size);

	pv->processes = workers;



	//
	// Filling the process vector buffer to be written to file
	//
	for (int ix = 0; ix < (long) ip[0]; ix++)
	{
		strncpy(pv->processV[ix].name,  "samsonWorker", sizeof(pv->processV[ix].name));
		strncpy(pv->processV[ix].host, ip[ix + 1], sizeof(pv->processV[ix].host));

		snprintf(pv->processV[ix].alias, sizeof(pv->processV[ix].alias), "Worker%02d", ix);

		pv->processV[ix].port = WORKER_PORT;
	}


	//
	// Saving the file to disk
	//
	ss::platformProcessesSave(pv);

	return 0;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	int err = 0;

	paConfig("prefix",                        (void*) "SSP_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	if (workers == 0)
	{
		printf("Number of workers> ");
		fflush(stdout);
		scanf("%d", &workers);

		ip[0] = (char*) workers;

		for (int ix = 0; ix < workers; ix++)
		{
			char ipaddress[64];

			printf("Please enter the IP address (or host name) for host number %d> ", ix + 1);
			fflush(stdout);
			scanf("%s", ipaddress);

			ip[ix + 1] = strdup(ipaddress);
		}
	}
	else
	{
		if (workers > (long) ip[0])
		{
			printf("Samson Platform Setup Error.\n"
				   "You specified %d workers with the '-workers' option, but gave only %d IP:s in the '-ip' option.\n"
				   "Please correct this error and try again.\n",
				   workers, (int) (long) ip[0]);
			err = 1;
		}
		else if (workers < (long) ip[0])
		{
			printf("Samson Platform Setup Error.\n"
				   "You specified %d workers with the '-workers' option, but gave %d IP:s in the '-ip' option.\n"
				   "Please correct this error and try again.\n",
				   workers, (int) (long) ip[0]);
			err = 2;
		}
	}

	
	if (err == 0)
		err = platformFileCreate(workers, ip);

	if (err != 0)
		printf("Operation terminated with failure %d.\n", err);

	return 0;
}
